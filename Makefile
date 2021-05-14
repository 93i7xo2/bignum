CFLAGS = -Wall -O2 -DNDEBUG

all: fibonacci

# Control the build verbosity
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
else
    Q := @
    VECHO = @printf
endif

OBJS := \
	fibonacci.o \
	bignum.o \
	apm.o \
	sqr.o \
	mul.o \
	format.o

OBJS1 := \
	test_bn.o \
	bignum.o \
	apm.o \
	sqr.o \
	mul.o \
	format.o

OBJS2 := \
	bn_cpython.o \
	test_bn_cpython.o

deps := $(OBJS:%.o=.%.o.d)

fibonacci: $(OBJS)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

test_bn: $(OBJS1)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

test_bn_cpython: $(OBJS2)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

test: test_bn_cpython test_bn
	@./test.sh

%.o: %.c
	@mkdir -p .$(DUT_DIR)
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

clean:
	rm -f $(OBJS) $(OBJS1) $(OBJS2) $(deps)
	$(RM) fibonacci test_bn test_bn_cpython
	rm -f *.dat *.png

-include $(deps)
