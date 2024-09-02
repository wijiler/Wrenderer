All:
	make -C ./Backend/
	cp ./Backend/Lib/JRV2BE.lib ./Frontend/libs/JRV2BE.lib
	make -C ./Frontend/

clean:
	rm -rf ./Backend/Lib/