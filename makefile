LDFLAGS = -Iinclude/ -Iinclude/libs/ -IC:/VulkanSDK/1.3.290.0/Include -L./libs/ -lvulkan-1 -lglfw3 -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt
CFLAGS = -x c -std=c99 -Wextra -Wall

Debug:
	clang $(CFLAGS) src/*.c -o main.exe $(LDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR
