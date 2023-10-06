#pragma once

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color);

D3DXVECTOR3 WorldToScreen(IDirect3DDevice9* dev, uint16_t playerXLoc, uint16_t playerYLoc, uint16_t itemXLoc, uint16_t itemYLoc);

D3DXVECTOR3 WorldToScreen2(IDirect3DDevice9* dev, uint16_t playerXLoc, uint16_t playerYLoc, uint16_t itemXLoc, uint16_t itemYLoc);

bool DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message);

D3DXVECTOR3 Lerp(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float t);