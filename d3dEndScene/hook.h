#pragma once

void Patch(BYTE* src, BYTE* dst, unsigned int len);

bool Hook(char* src, char* dst, unsigned int len);

char* TrampHook(char* src, char* dst, unsigned int len);