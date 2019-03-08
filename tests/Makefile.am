TESTS = check_gsia
check_PROGRAMS = check_gsia

check_gsia_SOURCES = \
	check_gsia.h \
	check_gsia_main.c \
	check_gsia_hyper.c \
	check_gsia_array.c \
	check_gsia_comm.c \
	check_gsia_io.c \
	check_gsia_utils.c

check_gsia_CFLAGS = $(CHECK_CFLAGS)
check_gsia_LDADD = $(top_builddir)/src/libgsia.la $(CHECK_LIBS)
