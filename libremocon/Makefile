CC = cc
PROGRAM = libremocon.a

$(PROGRAM):rc_center.o rc_next.o rc_pfree.o rc_start.o \
rc_analyze.o rc_decode.o rc_record.o rc_passign.o \
rc_receive.o rc_getopt.o rc_getch.o rc_memo.o rc_save.o \
rc_gettoken.o rc_getdata.o rc_speccmp.o rc_shot.o \
rc_getcommand.o rc_getbuttons.o

.c.o:
	$(CC) -c -I. $<
	ar r $(PROGRAM) $@

rc_center.o:rc_center.c remocon.h
rc_next.o:rc_next.c remocon.h
rc_pfree.o:rc_pfree.c remocon.h
rc_start.o:rc_start.c remocon.h
rc_analyze.o:rc_analyze.c remocon.h
rc_decode.o:rc_decode.c remocon.h
rc_record.o:rc_record.c remocon.h
rc_passign.o:rc_passign.c remocon.h
rc_receive.o:rc_receive.c remocon.h
rc_getopt.o:rc_getopt.c remocon.h
rc_getch.o:rc_getch.c remocon.h
rc_memo.o:rc_memo.c remocon.h
rc_save.o:rc_save.c remocon.h
rc_gettoken.o:rc_gettoken.c remocon.h
rc_getdata.o:rc_getdata.c remocon.h
rc_speccmp.o:rc_speccmp.c remocon.h
rc_shot.o:rc_shot.c remocon.h
rc_getcommand.o:rc_getcommand.c remocon.h
rc_getbuttons.o:rc_getbuttons.c remocon.h

install:$(PROGRAM)
	install -o root -g staff -m 0644 remocon.h /usr/local/include
	install -o root -g staff -m 0644 $(PROGRAM) /usr/local/lib

uninstall:
	rm -f /usr/local/include/remocon.h
	rm -f /usr/local/lib/$(PROGRAM)

clean:
	rm -f *.o
	rm -f $(PROGRAM)
