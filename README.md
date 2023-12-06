# ImViewGuizmo
A clone of [imoguizmo](https://github.com/fknfilewalker/imoguizmo) stripped of interactive functions and using an [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)-ish style of rendering. 

This also includes a fix for the stretching problem if the projection matrix's aspect ratio is not equal to 1:1.

A simple C++11 header only interactive orientation gizmo for ImGui. 
## Usage example
```c++
#include "ImViewGuizmo.hpp"

ImGui::Begin("RenderView", nullptr);
{
	wsize = ImGui::GetWindowSize();
	//All of your rendering view code...

	if (glIsTexture(m_frameBufferTex)) {
		ImGui::Image((ImTextureID)m_frameBufferTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
		//Draw ImGuizmo
		_drawGuizmo();

		//Draw ImViewGuizmo
		if (m_pCurrCamera != nullptr) {
			auto viewMatrix = (float*)glm::value_ptr(m_pCurrCamera->getViewMatrix());
			auto projMat = glm::value_ptr(m_pCurrCamera->getProjectionMatrix());

			ImViewGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + wsize.y - 150, 150, 150);
			ImViewGuizmo::drawGizmo(viewMatrix, projMat, (float)m_rendererWidth / (float)m_rendererHeight);
		}
	}
	ImGui::End();
}

```
Note: ImViewGuizmo::drawGizmo() must be called inside an ImGui window.

Drag
![drag_example](images/drag.gif)

[License (MIT)](https://github.com/fknfilewalker/imoguizmo/blob/main/LICENSE)