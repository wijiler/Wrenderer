VULKAN := $(subst \,/,$(subst C:\,C:/,${VULKAN_SDK}))

LDFLAGS := -I./include/ -I./include/libs/ -I$(VULKAN)/Include -L./libs/ -lvulkan-1 -lglfw3 -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt
CFLAGS := -x c -std=c99 -Wextra -Wall

Release:
	clang -c $(CFLAGS) ./src/*.c $(LDFLAGS) -DVK_USE_PLATFORM_WIN32_KHR
	mkdir -p Lib
	mv *.o Lib/
	llvm-ar \
	rc Lib/JRV2BE.lib \
	Lib/*.o
	rm -rf Lib/*.o

Clean: 
	rm -rf Lib/*.lib

Debug:
	clang -c $(CFLAGS) ./src/*.c $(LDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR
	mkdir -p Lib
	mv *.o Lib/
	llvm-ar \
		rc Lib/JRV2BE.lib \
		Lib/*.o
	rm -rf Lib/*.o