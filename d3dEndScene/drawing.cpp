#include "includes.h"

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT rect = { x, y, x + w, y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

//TODO: Figure out why this doesn't work right...This would be a much better approach then the bs in version2.
D3DXVECTOR3 WorldToScreen(IDirect3DDevice9* dev, uint16_t playerXLoc, uint16_t playerYLoc, uint16_t itemXLoc, uint16_t itemYLoc)
{
	D3DVIEWPORT9 viewport;
	dev->GetViewport(&viewport);
	float x = (itemXLoc * 20) + 10;
	float y = (itemYLoc * 20) + 10;
	float z = 0.0;

	float visibleWorldWidth = 400.f;
	float visibleWorldHeight = 356.f;

	D3DXVECTOR3 pos((x - playerXLoc) * (viewport.Width / visibleWorldWidth), (y - playerYLoc) * (viewport.Height / visibleWorldHeight), z);

	D3DXVECTOR3 screen;

	D3DXMATRIX world, projection, view;
	dev->GetTransform(D3DTS_WORLD, &world);
	dev->GetTransform(D3DTS_PROJECTION, &projection);
	dev->GetTransform(D3DTS_VIEW, &view);

	D3DXVec3Project(&screen, &pos, &viewport, &projection, &view, &world);

	screen.x = max(40, min(screen.x, viewport.Width) - 40);
	screen.y = max(40, min(screen.y, viewport.Height) - 40);

	return screen;
}

D3DXVECTOR3 WorldToScreen2(IDirect3DDevice9* dev, uint16_t playerXLoc, uint16_t playerYLoc, uint16_t itemXLoc, uint16_t itemYLoc)
{
	D3DVIEWPORT9 viewport;
	dev->GetViewport(&viewport);

	float scaleX = viewport.Width / zoomLevel;
	float scaleY = viewport.Height / (zoomLevel * 0.9f); //0.9 is because the window header probably need to check for full screen...

	float x = (itemXLoc * 20) + 10;
	float y = (itemYLoc * 20) + 10;
	float z = 0.0;

	// Calculate the screen position based on scaling factors
	float screenX = (x - playerXLoc) * scaleX + (viewport.Width / 2.0f);
	float screenY = (playerYLoc - y) * scaleY + (viewport.Height / 2.0f);
	float screenZ = 0.0f; // You can leave this as 0

	D3DXVECTOR3 screenPosition(screenX, screenY, screenZ);

	// Make sure the screen coordinates stay within the viewport
	screenPosition.x = max(40, min(screenPosition.x, viewport.Width - 40));
	screenPosition.y = max(40, min(screenPosition.y, viewport.Height - 40));

	return screenPosition;
}

bool DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message)
{   // Create a color for the text

	size_t len = 0;
	std::string temp = Message;
	std::vector<std::string> lines;
	char* token = std::strtok(const_cast<char*>(temp.c_str()), "\n");

	while (token != nullptr) 
	{
		lines.push_back(token);
		token = std::strtok(nullptr, "\n");
	}

	for (const std::string& line : lines)
		if (line.length() > len)
			len = line.length();

	len *= 6;

	D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, r, g, b);
	RECT rct;
	rct.left = x - len;
	rct.right = x + len;
	rct.top = y - 20;
	rct.bottom = rct.top + 50;
	font->DrawTextA(NULL, Message, -1, &rct, 0, fontColor);

	return true;
}

D3DXVECTOR3 Lerp(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float t)
{
	return start + t * (end - start);
}