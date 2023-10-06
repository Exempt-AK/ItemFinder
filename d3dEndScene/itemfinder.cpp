#include "includes.h"

std::unordered_map<int, ItemData> watchList;
std::unordered_map<iv2, ItemData> drawPositions;

uint16_t playerX, playerY, spellEffects = 0;
uintptr_t zoomAddr = 0x00560870;
uintptr_t ptrAddr = 0x00A2D014;
uintptr_t opAddr = 0;
BYTE bytes[5000];
float zoomLevel = 0;


void SetDrawPositionsFalse()
{
    for (auto& pos : drawPositions)
        pos.second.draw = false;
}

void ClearFalseDrawPositions()
{
    for (auto it = drawPositions.begin(); it != drawPositions.end(); )
        if (it->second.draw == false)
            it = drawPositions.erase(it);
        else
            ++it; 

}

void ParseItemConfig() 
{
	char moduleFileName[MAX_PATH];
	if (GetModuleFileName(NULL, moduleFileName, MAX_PATH) == 0) 
	{
		std::cerr << "Error getting module file name." << std::endl;
		return;
	}

	std::string modulePath = moduleFileName;
	size_t lastSlash = modulePath.find_last_of('\\');
	std::string directory = modulePath.substr(0, lastSlash + 1);
	std::string filePath = directory + "ItemFinder\\ItemConfig.txt";

    std::ifstream file(filePath);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        std::string id, name, watch;
        std::stringstream iss(line);
        std::getline(iss, id, ',');
        std::getline(iss, name, ',');
        std::getline(iss, watch);

		if (watch == "true")
			watchList[std::stoi(id)] = ItemData(name, true);
		else if (watch == "false")
			watchList[std::stoi(id)] = ItemData(name, false);
    }
    
    file.close();
}

void GetUpdateDataMemoryAddress(HANDLE hProcess)
{
	ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(ptrAddr), &opAddr, sizeof(opAddr), NULL);
}

void UpdateWorldData(HANDLE hProcess)
{
	ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(zoomAddr), &zoomLevel, sizeof(zoomLevel), NULL);
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(opAddr), bytes, sizeof(bytes), NULL);
	
	SetDrawPositionsFalse();

	if (bytes[0] == 0x03)
	{
		int offset = 0;
		//unsigned char* byteArray;
		playerX = ReadUShort(bytes, offset + 2);
		playerY = ReadUShort(bytes, offset + 4);
		spellEffects = ReadUShort(bytes, offset + 6);

		int numItems = bytes[12];
		offset += 26;
		if (numItems > 0)
		{
			iv2 pos;
			//printf("numItems: %i\n", numItems);
			for (int i = 0; i < numItems; i++)
			{
				std::string itemNames = "";
				if (bytes[offset] == 0x01)
				{
					unsigned short x = ReadUShort(bytes, offset + 2);
					unsigned short y = ReadUShort(bytes, offset + 4);
					unsigned short id = ReadUShort(bytes, offset + 6);

					const auto& item = watchList.find(id);
					if (item != watchList.end())
					{
						pos.x = x;
						pos.y = y;
						if(item->second.draw == true)
							drawPositions[pos] = ItemData(item->second.name + ' ' + std::to_string(id), true);
					}
					else if (id){
						pos.x = x;
						pos.y = y;
						drawPositions[pos] = ItemData("Unknown item: " + id, true);
					}

					offset += 10;
					//printf("%i, %i, %i\n", x, y, id);
				}
				if (bytes[offset] == 0x02)
				{
					unsigned short x = ReadUShort(bytes, offset + 2);
					unsigned short y = ReadUShort(bytes, offset + 4);

					unsigned short id1 = ReadUShort(bytes, offset + 6);
					unsigned short id2 = ReadUShort(bytes, offset + 8);
					unsigned short id3 = ReadUShort(bytes, offset + 10);

					const auto& item1 = watchList.find(id1);
					const auto& item2 = watchList.find(id2);
					const auto& item3 = watchList.find(id3);

					if (item1 != watchList.end())
					{
						if (item1->second.draw)
							itemNames = itemNames + item1->second.name + ' ' + std::to_string(id1) + "\n";
					}
					else if (id1){
						pos.x = x;
						pos.y = y;
						drawPositions[pos] = ItemData("Unknown item: " + id1, true);
					}
					if (item2 != watchList.end())
					{
						if (item2->second.draw)
							itemNames = itemNames + item2->second.name + ' ' + std::to_string(id2) + "\n";
					}
					else if (id2){
						pos.x = x;
						pos.y = y;
						drawPositions[pos] = ItemData("Unknown item: " + id2, true);
					}
					if (item3 != watchList.end())
					{
						if (item3->second.draw)
							itemNames = itemNames + item3->second.name + ' ' + std::to_string(id3) + "\n";
					}
					else if (id3){
						pos.x = x;
						pos.y = y;
						drawPositions[pos] = ItemData("Unknown item: " + id3, true);
					}

					if (item1 != watchList.end() || item2 != watchList.end() || item3 != watchList.end())
					{
						if (item1->second.draw || item2->second.draw || item3->second.draw)
						{
							//printf("Found: %s\n", item->second.c_str());
							iv2 pos;// = WorldToScreen(px, py, x, y);
							pos.x = x;
							pos.y = y;
							drawPositions[pos] = ItemData(itemNames, true);
						}
					}

					offset += 14;
					//printf("%i, %i, %i, %i, %i\n", x, y, id1, id2, id3);
				}
				if (bytes[offset] == 0x03)
				{
					//this is a pain, seems like it may be similar to unit updates
					//t1 n/w just outside has 1-2 of these 0x03 1f and 3f i believe.
					/*unsigned short x = ReadUShort(bytes, offset + 3);
					unsigned short y = ReadUShort(bytes, offset + 5);
					printf("numItems: %i, i: %i\n", numItems, i);
					printf("item 0x03 unknown: %i %i, player: %i %i\n", x, y, playerX, playerY);
					for (int i = 0; i < 40; i++) {
						printf("%02X ", bytes[offset + i]);
					}
					printf("\n");*/
					offset += 21;
				}
			}
		}
		ClearFalseDrawPositions();
	}
}