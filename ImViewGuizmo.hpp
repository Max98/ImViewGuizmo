/*
MIT License

Copyright(c) 2022 Lukas Lipp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <imgui.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace ImViewGuizmo {
	struct ContextConfig {
		float mX = 0.f;
		float mY = 0.f;
		float mWidth = 0.f;
		float mHeight = 0.f;
	
		ImDrawList* mDrawList;
	};

	static ContextConfig contextConfig;

	namespace internal {
		inline ImVec4 multiply(const float* const m, const ImVec4& v)
		{
			const float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w;
			const float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w;
			const float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
			const float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;
			return { x, y, z, w };
		}
		inline void multiply(const float* const l, const float* const r, float* out)
		{
			out[0] = l[0] * r[0] + l[1] * r[4] + l[2] * r[8] + l[3] * r[12];
			out[1] = l[0] * r[1] + l[1] * r[5] + l[2] * r[9] + l[3] * r[13];
			out[2] = l[0] * r[2] + l[1] * r[6] + l[2] * r[10] + l[3] * r[14];
			out[3] = l[0] * r[3] + l[1] * r[7] + l[2] * r[11] + l[3] * r[15];

			out[4] = l[4] * r[0] + l[5] * r[4] + l[6] * r[8] + l[7] * r[12];
			out[5] = l[4] * r[1] + l[5] * r[5] + l[6] * r[9] + l[7] * r[13];
			out[6] = l[4] * r[2] + l[5] * r[6] + l[6] * r[10] + l[7] * r[14];
			out[7] = l[4] * r[3] + l[5] * r[7] + l[6] * r[11] + l[7] * r[15];

			out[8] = l[8] * r[0] + l[9] * r[4] + l[10] * r[8] + l[11] * r[12];
			out[9] = l[8] * r[1] + l[9] * r[5] + l[10] * r[9] + l[11] * r[13];
			out[10] = l[8] * r[2] + l[9] * r[6] + l[10] * r[10] + l[11] * r[14];
			out[11] = l[8] * r[3] + l[9] * r[7] + l[10] * r[11] + l[11] * r[15];

			out[12] = l[12] * r[0] + l[13] * r[4] + l[14] * r[8] + l[15] * r[12];
			out[13] = l[12] * r[1] + l[13] * r[5] + l[14] * r[9] + l[15] * r[13];
			out[14] = l[12] * r[2] + l[13] * r[6] + l[14] * r[10] + l[15] * r[14];
			out[15] = l[12] * r[3] + l[13] * r[7] + l[14] * r[11] + l[15] * r[15];
		}

		inline void drawPositiveLine(const ImVec2 center, const ImVec2 axis, const ImU32 color, const float radius, const float thickness, const char* text) {
			const auto lineEndPositive = ImVec2(center.x + axis.x, center.y + axis.y);

			contextConfig.mDrawList->AddLine(center, lineEndPositive, color, thickness);
			contextConfig.mDrawList->AddCircleFilled(lineEndPositive, radius, color);
			const auto textPosX = ImVec2(floor(lineEndPositive.x - 3.0f), floor(lineEndPositive.y - 8.0f));
			contextConfig.mDrawList->AddText(textPosX, IM_COL32_WHITE, text);
		}

		inline void drawNegativeLine(const ImVec2 center, const ImVec2 axis, const ImU32 color, const float radius) {
			const auto lineEndNegative = ImVec2(center.x - axis.x, center.y - axis.y);
			contextConfig.mDrawList->AddCircleFilled(lineEndNegative, radius, color);
		}
	}

	inline void SetRect(float x, float y, float width, float height)
	{
		contextConfig.mX = x;
		contextConfig.mY = y;
		contextConfig.mWidth = width;
		contextConfig.mHeight = height;
	}

	inline void drawGizmo(float* const viewMatrix, const float* const projectionMatrix, const float aspectRatio, const float pivotDistance = 0.0f, const bool background = false) {
		float viewProjection[16];
		internal::multiply(viewMatrix, projectionMatrix, viewProjection);
		{ viewProjection[1] *= -1; viewProjection[5] *= -1; viewProjection[9] *= -1; viewProjection[13] *= -1; } // Flip Y

		// axis
		const float axisLength = contextConfig.mWidth * 0.25;

		//Fix for stretched rendering of the guizmo
		viewProjection[0] *= aspectRatio;
		viewProjection[8] *= aspectRatio;

		const ImVec4 xAxis = internal::multiply(viewProjection, ImVec4(axisLength, 0, 0, 0));
		const ImVec4 yAxis = internal::multiply(viewProjection, ImVec4(0, axisLength, 0, 0));
		const ImVec4 zAxis = internal::multiply(viewProjection, ImVec4(0, 0, axisLength, 0));

		const bool interactive = pivotDistance > 0.0f;
		const ImVec2 mousePos = ImGui::GetIO().MousePos;

		const auto center = ImVec2(contextConfig.mX + contextConfig.mWidth * 0.5, contextConfig.mY + contextConfig.mHeight * 0.5);

		const float positiveRadius = 9.0f;	// for size 120 -> 9;
		const float negativeRadius = 6.0f;	// for size 120 -> 6;
		const bool redPositiveCloser = 0.0f >= xAxis.w;
		const bool greenPositiveCloser = 0.0f >= yAxis.w;
		const bool bluePositiveCloser = 0.0f >= zAxis.w;

		constexpr ImU32 redFrontColor = IM_COL32(255, 54, 83, 255);
		constexpr ImU32 redBackColor = IM_COL32(154, 57, 71, 255);
		constexpr ImU32 greenFrontColor = IM_COL32(138, 219, 0, 255);
		constexpr ImU32 greenBackColor = IM_COL32(98, 138, 34, 255);
		constexpr ImU32 blueFrontColor = IM_COL32(44, 143, 255, 255);
		constexpr ImU32 blueBackColor = IM_COL32(52, 100, 154, 255);

		// sort axis based on distance
		// 0 : +x axis, 1 : +y axis, 2 : +z axis, 3 : -x axis, 4 : -y axis, 5 : -z axis
		std::vector<std::pair<int, float>> pairs = { {0, xAxis.w}, {1, yAxis.w}, {2, zAxis.w}, {3, -xAxis.w}, {4, -yAxis.w}, {5, -zAxis.w} };
		sort(pairs.begin(), pairs.end(), [=](const std::pair<int, float>& aA, const std::pair<int, float>& aB) { return aA.second > aB.second; });

		contextConfig.mDrawList = ImGui::GetWindowDrawList();

		// draw back first
		const float lineThickness = 2.0f; // for size 120 -> 2.0f;
		for (const auto& [fst, snd] : pairs) {
			switch (fst) {
			case 0: // +x axis
				internal::drawPositiveLine(center, ImVec2(xAxis.x, xAxis.y), redPositiveCloser ? redFrontColor : redBackColor, positiveRadius, lineThickness, "X");
				continue;
			case 1: // +y axis
				internal::drawPositiveLine(center, ImVec2(yAxis.x, yAxis.y), greenPositiveCloser ? greenFrontColor : greenBackColor, positiveRadius, lineThickness, "Y");
				continue;
			case 2: // +z axis
				internal::drawPositiveLine(center, ImVec2(zAxis.x, zAxis.y), bluePositiveCloser ? blueFrontColor : blueBackColor, positiveRadius, lineThickness, "Z");
				continue;
			case 3: // -x axis
				internal::drawNegativeLine(center, ImVec2(xAxis.x, xAxis.y), !redPositiveCloser ? redFrontColor : redBackColor, negativeRadius);
				continue;
			case 4: // -y axis
				internal::drawNegativeLine(center, ImVec2(yAxis.x, yAxis.y), !greenPositiveCloser ? greenFrontColor : greenBackColor, negativeRadius);
				continue;
			case 5: // -z axis
				internal::drawNegativeLine(center, ImVec2(zAxis.x, zAxis.y), !bluePositiveCloser ? blueFrontColor : blueBackColor, negativeRadius);
				continue;
			default: break;
			}
		}

	}
}