#pragma once
struct ItemData
{
	ItemData() {}

	ItemData(std::string _name, bool _draw)
	{
		name = _name;
		draw = _draw;
	}
	std::string name = "Not Set";
	bool draw = false;
};

struct iv2
{
	int x, y;

	bool operator==(const iv2& rhs) const { return x == rhs.x && y == rhs.y; }
	auto operator<=>(const iv2&) const = default;
};

namespace std {
	template<>
	struct hash<iv2> {
		size_t operator()(const iv2& k) const {
			// Use a simple hash combining x and y
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
		}
	};
}


extern std::unordered_map<int, ItemData> watchList;
extern std::unordered_map<iv2, ItemData> drawPositions;
extern uint16_t playerX, playerY, spellEffects;
extern uintptr_t ptrAddr, opAddr, zoomAddr;
extern BYTE bytes[5000];
extern float zoomLevel;

void SetDrawPositionsFalse();
void ClearFalseDrawPositions();
void ParseItemConfig();
void GetUpdateDataMemoryAddress(HANDLE hProcess);
void UpdateWorldData(HANDLE hProcess);
