VULKAN := $(subst \,/,$(subst C:\,C:/,${VULKAN_SDK}))
EXELDFLAGS := -I../../include/ -I../../include/libs/ -I$(VULKAN)/Include -L../../libs/ -L../../Lib/ -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt -lvulkan-1 -lglfw3 -lJRV2BE.lib
CFLAGS := -x c -std=c99 -Wextra -Wall

Release:
	clang $(CFLAGS) hellotriangle.c $(EXELDFLAGS) -DVK_USE_PLATFORM_WIN32_KHR -o ../hellotriangle.exe -O3
Debug:
	clang $(CFLAGS) hellotriangle.c $(EXELDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR -o ../hellotriangle.exe -g 

