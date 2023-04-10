OBJ_DIR 	= obj_files
CFLAGS 		= -ansi -Wall -pedantic
APP_NAME 	= assembler_exe
TEST_DIR	= test_files
ifdef DEBUG
CFLAGS+=-g
endif

ifdef MEM_CHECK
CFLAGS+=-fsanitize=address
endif

$(APP_NAME): obj_files/sst.o obj_files/errors_checks.o obj_files/handle_parse.o obj_files/preprocessor.o obj_files/dynarray.o obj_files/bmc_to_obj.o obj_files/assembler.o obj_files/main.o
	gcc $(CFLAGS) obj_files/sst.o obj_files/errors_checks.o obj_files/handle_parse.o obj_files/preprocessor.o obj_files/dynarray.o obj_files/bmc_to_obj.o obj_files/assembler.o obj_files/main.o -o $@

obj_files/sst.o: sst/sst.c sst/sst.h sst/errors_checks.h sst/handle_parse.h
	gcc -c $(CFLAGS) sst/sst.c -o obj_files/sst.o

obj_files/errors_checks.o: sst/errors_checks.c sst/errors_checks.h sst/sst.h sst/handle_parse.h
	gcc -c $(CFLAGS) sst/errors_checks.c -o obj_files/errors_checks.o

obj_files/handle_parse.o: sst/handle_parse.c sst/errors_checks.h sst/sst.h
	gcc -c $(CFLAGS) sst/handle_parse.c -o obj_files/handle_parse.o

obj_files/preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h
	gcc -c $(CFLAGS) preprocessor/preprocessor.c -o obj_files/preprocessor.o

obj_files/dynarray.o: dynarray/dynarray.c dynarray/dynarray.h
	gcc -c $(CFLAGS) dynarray/dynarray.c -o obj_files/dynarray.o

obj_files/bmc_to_obj.o: bmc_to_object/bmc_to_obj.c bmc_to_object/bmc_to_obj.h dynarray/dynarray.h
	gcc -c $(CFLAGS) bmc_to_object/bmc_to_obj.c -o obj_files/bmc_to_obj.o

obj_files/assembler.o: assembler/assembler.c dynarray/dynarray.h bmc_to_object/bmc_to_obj.h sst/sst.h
	gcc -c $(CFLAGS) assembler/assembler.c -o obj_files/assembler.o

obj_files/main.o: assembler/main.c assembler/assembler.h
	gcc -c $(CFLAGS) assembler/main.c -o obj_files/main.o

clean_outputs:
	rm -f $(TEST_DIR)/*.ent $(TEST_DIR)/*.ext $(TEST_DIR)/*.ob $(TEST_DIR)/*.am
clean: clean_outputs
	rm -f $(OBJ_DIR)/*.o
	rm -f $(APP_NAME)

submit_maman: clean
	zip -r mmmn14.zip *



