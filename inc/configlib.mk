
include ../inc/config.mk

$(LIB): $(OBJS)
	$(AR) rcuv $(LIB) $(OBJS)
