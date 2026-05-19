#pragma once

#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <cmath>
#include <cstdint>

#include <u8g2lib.h>

#include "Vec.h"
#include "Mat.h"

class Renderer {
private:
	struct Window {
		int width;
		int height;
	};

	U8G2* u8g2Display = nullptr;
	Window window;

	std::vector<float> depthBuffer;

	Mat4 viewMat;

private:
    

	float cross2D(const Vec2& v1, const Vec2& v2) const;
	bool isTopLeft(const Vec2& edge) const;
    
	void toScreenSpace(std::array<Vec4, 3>& vertices) const;
	const std::vector<uint8_t>& getFramebuffer() const;

public:
	Renderer(U8G2& display, int width, int height);
	
	bool isRunning() const;
	void display();
	void drawPixel(const Pixel& pixel, uint8_t brightness);
	void drawTriangle(const std::array<Vec3, 3>& originalVertices, float brightness, const Mat4& modelMat);
    void drawModel(const std::vector<float>& VBO, const std::vector<uint32_t>& EBO, float brightness, const Mat4& modelMat);

	void setViewMat(const Mat4& viewMat);
};