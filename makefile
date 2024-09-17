VULKAN := $(subst \,/,$(subst C:\,C:/,${VULKAN_SDK}))

LDFLAGS := -I./include/ -I./include/libs/ -I$(VULKAN)/Include -L./libs/
EXELDFLAGS := -I./include/ -I./include/libs/ -I$(VULKAN)/Include -L./libs/ -luser32 -lmsvcrt -lgdi32 -lshell32 -llibcmt -lvulkan-1 -lglfw3
CFLAGS := -x c -std=c99 -Wextra -Wall

Release:
	clang -c $(CFLAGS) ./src/*.c $(LDFLAGS) -DVK_USE_PLATFORM_WIN32_KHR
	mkdir -p Lib
	mv *.o Lib/
	llvm-ar \
	rc Lib/JRV2BE.lib \
	Lib/*.o
	rm -rf Lib/*.o
	clang $(CFLAGS) ./src/*.c $(EXELDFLAGS) -DVK_USE_PLATFORM_WIN32_KHR -o main.exe -O3 


Clean: 
	rm -rf Lib/*.lib *.exe *.ilk *.pdb *.exp *.lib

Debug:
	clang -c $(CFLAGS) ./src/*.c $(LDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR
	mkdir -p Lib
	mv *.o Lib/
	llvm-ar \
		rc Lib/JRV2BE.lib \
		Lib/*.o
	rm -rf Lib/*.o
	clang $(CFLAGS) ./src/*.c $(EXELDFLAGS) -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR -o main.exe -g 