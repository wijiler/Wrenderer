VULKAN := $(subst \,/,$(subst C:\,C:/,${VULKAN_SDK}))

CFLAGS = -x c++ -std=c++17 -Wextra -Wall -I../Backend/include/ -I$(VULKAN)/Include -I./Include/ -I../Backend/Include/libs/

LDFLAGS = -L./libs/ -lJRV2BE -lvulkan-1 -lglfw3 -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt

All:
	clang++ $(CFLAGS) src/*.cpp -o main.exe $(LDFLAGS)