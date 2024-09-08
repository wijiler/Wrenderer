VULKAN := $(subst \,/,$(subst C:\,C:/,${VULKAN_SDK}))

CFLAGS = -x c++ -std=c++17 -Wextra -Wall -I../Backend/include/ -I$(VULKAN)/Include -I./Include/ -I../Backend/Include/libs/

LDFLAGS = -L./libs/ -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt -lvulkan-1 -lglfw3 -lJRV2BE 

Release:
	clang++ $(CFLAGS) src/*.cpp -o main.exe $(LDFLAGS) -O3
Debug:
	clang++ $(CFLAGS) src/*.cpp -o main.exe $(LDFLAGS) -g
Clean:
	rm -rf *.exe *.pdb *.ilk libs/JRV2BE.lib