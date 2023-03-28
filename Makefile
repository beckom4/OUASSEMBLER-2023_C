assembler_run: obj_files/sst.o obj_files/errors_checks.o obj_files/handle_parse.o obj_files/preprocessor.o obj_files/dynarray.o obj_files/bmc_to_obj.o obj_files/assembler.o obj_files/main.o
	gcc -ansi -Wall -pedantic obj_files/sst.o obj_files/errors_checks.o obj_files/handle_parse.o obj_files/preprocessor.o obj_files/dynarray.o obj_files/bmc_to_obj.o obj_files/assembler.o obj_files/main.o -o assembler_run
obj_files/sst.o: sst/sst.c sst/sst.h sst/errors_checks.h sst/handle_parse.h
	gcc -c -ansi -Wall -pedantic sst/sst.c -o obj_files/sst.o
obj_files/errors_checks.o: sst/errors_checks.c sst/errors_checks.h sst/sst.h sst/handle_parse.h
	gcc -c -ansi -Wall -pedantic sst/errors_checks.c -o obj_files/errors_checks.o
obj_files/handle_parse.o: sst/handle_parse.c sst/errors_checks.h sst/sst.h
	gcc -c -ansi -Wall -pedantic sst/handle_parse.c -o obj_files/handle_parse.o
obj_files/preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h
	gcc -c -ansi -Wall -pedantic preprocessor/preprocessor.c -o obj_files/preprocessor.o
obj_files/dynarray.o: dynarray/dynarray.c dynarray/dynarray.h
	gcc -c -ansi -Wall -pedantic dynarray/dynarray.c -o obj_files/dynarray.o
obj_files/bmc_to_obj.o: bmc_to_object/bmc_to_obj.c bmc_to_object/bmc_to_obj.h dynarray/dynarray.h
	gcc -c -ansi -Wall -pedantic bmc_to_object/bmc_to_obj.c -o obj_files/bmc_to_obj.o
obj_files/assembler.o: assembler/assembler.c dynarray/dynarray.h bmc_to_object/bmc_to_obj.h sst/sst.h
	gcc -c -ansi -Wall -pedantic assembler/assembler.c -o obj_files/assembler.o
obj_files/main.o: assembler/main.c assembler/assembler.h
	gcc -c -ansi -Wall -pedantic assembler/main.c -o obj_files/main.o
