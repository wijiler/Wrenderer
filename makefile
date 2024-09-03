All:
	make -C ./Backend/
	cp ./Backend/Lib/JRV2BE.lib ./Frontend/libs/JRV2BE.lib
	make -C ./Frontend/

DebugLib:
	make -C ./Backend/ debug
	./Backend/main.exe

clean:
	rm -rf ./Backend/Lib/