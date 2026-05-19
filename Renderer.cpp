#include "Renderer.h"

Renderer::Renderer(U8G2& display, int width, int height) : depthBuffer(width * height), u8g2Display(&display) {
	// Pure software renderer: initialize dimensions and buffers
	window.width = width;
	window.height = height;


	std::fill(depthBuffer.begin(), depthBuffer.end(), 1.0f);
}

// No windowing or GL — resize must be handled externally if needed.


bool Renderer::isRunning() const {
	// Without a windowing system, consider renderer always running.
	return true;
}

void Renderer::display() {
	if (!u8g2Display) {
		return;
	}

	// Send the buffered frame to the physical display and prepare a clean buffer for the next frame.
	u8g2Display->sendBuffer();
	u8g2Display->clearBuffer();

	std::fill(depthBuffer.begin(), depthBuffer.end(), 1.0f);
}

void Renderer::drawPixel(const Pixel& pixel, uint8_t brightness) {
	if (!u8g2Display) {
		return;
	}

	if (pixel.x < 0 || pixel.x >= window.width || pixel.y < 0 || pixel.y >= window.height) {
		return;
	}

	u8g2Display->setDrawColor(brightness ? 1 : 0);
	u8g2Display->drawPixel(pixel.x, pixel.y);
}

float Renderer::cross2D(const Vec2& v1, const Vec2& v2) const {
	return v1.x * v2.y - v1.y * v2.x;
}

bool Renderer::isTopLeft(const Vec2& edge) const {
	bool left = (edge.y < 0) ? true : false;
	bool top = (edge.y == 0 && edge.x < 0) ? true : false;

	return left || top;
}

// Texture sampling removed — renderer uses single brightness value per triangle/pixel.

void Renderer::toScreenSpace(std::array<Vec4, 3>& vertices) const {
	for (auto& vertex : vertices) {
		vertex.x = std::round(window.width / 2.0 + vertex.x * window.width / 2.0);
		vertex.y = std::round(window.height / 2.0 + vertex.y * window.height / 2.0);
	}
}

void Renderer::setViewMat(const Mat4& viewMat) {
	this->viewMat = viewMat;
}

void Renderer::drawTriangle(const std::array<Vec3, 3>& originalVertices, float brightness, const Mat4& modelMat) {
	std::array<Vec4, 3> vertices = { originalVertices[0], originalVertices[1], originalVertices[2] };
	Mat4 perspective((float)window.width / window.height, 90, 0.1, 100);
	std::array<float, 3> depths;

	for (size_t i = 0; i < vertices.size(); ++i) {
		vertices[i] = perspective * modelMat * vertices[i];
		depths[i] = vertices[i].w;
		vertices[i] /= vertices[i].w;
	}

	toScreenSpace(vertices);
	
	std::array<Vec2, 3> edges = { vertices[1] - vertices[0], vertices[2] - vertices[1], vertices[0] - vertices[2]};
	std::array<float, 3> biases;

	int minX = std::floor(std::min({ vertices[0].x, vertices[1].x , vertices[2].x }));
	int maxX = std::ceil(std::max({ vertices[0].x, vertices[1].x , vertices[2].x }));
	int minY = std::floor(std::min({ vertices[0].y, vertices[1].y , vertices[2].y }));
	int maxY = std::ceil(std::max({ vertices[0].y, vertices[1].y , vertices[2].y }));

	minX = std::max(minX, 0);
	maxX = std::min(maxX, window.width - 1);
	minY = std::max(minY, 0);
	maxY = std::min(maxY, window.height - 1);

	if (minX > maxX || minY > maxY) {
		return;
	}

	biases[0] = (isTopLeft(edges[0])) ? 0 : -1;
	biases[1] = (isTopLeft(edges[1])) ? 0 : -1;
	biases[2] = (isTopLeft(edges[2])) ? 0 : -1;

	Pixel firstPixel(minX + 0.5f, minY + 0.5f);

	Vec2 v1ToFP = firstPixel - vertices[0];
	Vec2 v2ToFP = firstPixel - vertices[1];
	Vec2 v3ToFP = firstPixel - vertices[2];

	float firstCross1 = cross2D(edges[0], v1ToFP) + biases[0];
	float firstCross2 = cross2D(edges[1], v2ToFP) + biases[1];
	float firstCross3 = cross2D(edges[2], v3ToFP) + biases[2];

	float deltaX1 = vertices[0].y - vertices[1].y;
	float deltaY1 = vertices[1].x - vertices[0].x;

	float deltaX2 = vertices[1].y - vertices[2].y;
	float deltaY2 = vertices[2].x - vertices[1].x;

	float deltaX3 = vertices[2].y - vertices[0].y;
	float deltaY3 = vertices[0].x - vertices[2].x;

	float area = cross2D(vertices[1] - vertices[0], vertices[2] - vertices[0]);
	int sign = (area >= 0) ? 1 : -1;

	for (int y = minY, yCounter = 0; y <= maxY; y++, yCounter++ ) {
		for (int x = minX, xCounter = 0; x <= maxX; x++, xCounter++ ) {
			Pixel pixel(x, y);
			
			float cross1 = firstCross1 + xCounter * deltaX1 + yCounter * deltaY1;
			float cross2 = firstCross2 + xCounter * deltaX2 + yCounter * deltaY2;
			float cross3 = firstCross3 + xCounter * deltaX3 + yCounter * deltaY3;

			if (cross1 * sign >= 0 && cross2 * sign >= 0 && cross3 * sign >= 0) {
				float alfa = cross2 / area;
				float beta = cross3 / area;
				float gama = cross1 / area;

				Vec3 bcWeights(alfa, beta, gama);

				float depth = vertices[0].z * alfa + vertices[1].z * beta + vertices[2].z * gama;
				int index = pixel.y * window.width + pixel.x;

				if (depth >= 0 && depth <= depthBuffer[index]) {
					depthBuffer[index] = depth;
                    if (brightness < 0) brightness = 0;
                    if (brightness > 1) brightness = 1;
					uint8_t b = static_cast<uint8_t>(brightness * 255.0f);
					drawPixel(pixel, b);
				}
			}
		}
	}
}

void Renderer::drawModel(const std::vector<float>& VBO, const std::vector<uint32_t>& EBO, float brightness, const Mat4& modelMat) {
	for (size_t i = 0; i < EBO.size(); i += 3) {
        std::array<Vec3, 3> triangleVertices = {
            Vec3(VBO[EBO[i] * 3], VBO[EBO[i] * 3 + 1], VBO[EBO[i] * 3 + 2]),
            Vec3(VBO[EBO[i + 1] * 3], VBO[EBO[i + 1] * 3 + 1], VBO[EBO[i + 1] * 3 + 2]),
            Vec3(VBO[EBO[i + 2] * 3], VBO[EBO[i + 2] * 3 + 1], VBO[EBO[i + 2] * 3 + 2])
        };

        drawTriangle(triangleVertices, brightness, modelMat);
    }
}