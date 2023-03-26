sst_fianl_test: sst.o errors_checks.o handle_parse.o preprocessor.o dynarray.o bmc.o assembler.o
	gcc -ansi -Wall -pedantic sst.o errors_checks.o handle_parse.o preprocessor.o dynarray.o bmc.o assembler.o -o sst_final_test
sst.o: sst.c sst.h errors_checks.h handle_parse.h
	gcc -c -ansi -Wall -pedantic sst.c -o sst.o
errors_checks.o: errors_checks.c errors_checks.h sst.h handle_parse.h
	gcc -c -ansi -Wall -pedantic errors_checks.c -o errors_checks.o
handle_parse.o: handle_parse.c errors_checks.h sst.h
	gcc -c -ansi -Wall -pedantic handle_parse.c -o handle_parse.o
preprocessor.o: preprocessor.c preprocessor.h
	gcc -c -ansi -Wall -pedantic preprocessor.c -o preprocessor.o
dynarray.o: dynarray.c dynarray.h
	gcc -c -ansi -Wall -pedantic dynarray.c -o dynarray.o
bmc.o: bmc.c bmc.h dynarray.h
	gcc -c -ansi -Wall -pedantic bmc.c -o bmc.o
assembler.o: assembler.c dynarray.h bmc.h sst.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
