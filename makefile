All:
	make -C ./Backend/
	cp ./Backend/Lib/JRV2BE.lib ./Frontend/libs/JRV2BE.lib
	make -C ./Frontend/

Debug:
	make -C ./Backend/ Debug
	make -C ./Frontend/ Debug

DebugLib:
	make -C ./Backend/ Debug

clean:
	make -C ./Backend/ Clean
	make -C ./Frontend/ Clean