VKSDK := ${VULKAN_SDK}
VULKAN := $(subst \,/,$(subst C:\,C:/,$(VKSDK)))

LDFLAGS := -Iinclude/ -Iinclude/libs/ -I$(VULKAN)/Include -L./libs/ -lvulkan-1 -lglfw3 -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt
CFLAGS := -x c -std=c99 -Wextra -Wall

Debug:
	clang $(CFLAGS) src/*.c -o main.exe $(LDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR
