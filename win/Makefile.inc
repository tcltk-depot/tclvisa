#
# Platform-independend definitions
# This file is included into a main Makefile.vc
#

SRCDIR=../../src/tclvisa
OBJDIR=.
OUTDIR=.

SRCS=\
$(SRCDIR)\find.c \
$(SRCDIR)\visa_constants.c \
$(SRCDIR)\open_default_rm.c \
$(SRCDIR)\clear.c \
$(SRCDIR)\visa_channel.c \
$(SRCDIR)\set_attribute.c \
$(SRCDIR)\unlock.c \
$(SRCDIR)\visa_utils.c \
$(SRCDIR)\lock.c \
$(SRCDIR)\read_to_file.c \
$(SRCDIR)\open.c \
$(SRCDIR)\write_from_file.c \
$(SRCDIR)\get_attribute.c \
$(SRCDIR)\parse_rsrc.c \
$(SRCDIR)\tcl_utils.c \
$(SRCDIR)\get_last_error.c \
$(SRCDIR)\assert_trigger.c \
$(SRCDIR)\assert_intr_signal.c \
$(SRCDIR)\assert_util_signal.c \
$(SRCDIR)\gpib_command.c \
$(SRCDIR)\gpib_control_atn.c \
$(SRCDIR)\gpib_control_ren.c \
$(SRCDIR)\gpib_pass_control.c \
$(SRCDIR)\gpib_send_ifc.c \
$(SRCDIR)\tclvisa.c

OBJS=\
$(OBJDIR)\find.obj \
$(OBJDIR)\visa_constants.obj \
$(OBJDIR)\open_default_rm.obj \
$(OBJDIR)\clear.obj \
$(OBJDIR)\visa_channel.obj \
$(OBJDIR)\set_attribute.obj \
$(OBJDIR)\unlock.obj \
$(OBJDIR)\visa_utils.obj \
$(OBJDIR)\lock.obj \
$(OBJDIR)\read_to_file.obj \
$(OBJDIR)\open.obj \
$(OBJDIR)\write_from_file.obj \
$(OBJDIR)\get_attribute.obj \
$(OBJDIR)\parse_rsrc.obj \
$(OBJDIR)\tcl_utils.obj \
$(OBJDIR)\get_last_error.obj \
$(OBJDIR)\assert_trigger.obj \
$(OBJDIR)\assert_intr_signal.obj \
$(OBJDIR)\assert_util_signal.obj \
$(OBJDIR)\gpib_command.obj \
$(OBJDIR)\gpib_control_atn.obj \
$(OBJDIR)\gpib_control_ren.obj \
$(OBJDIR)\gpib_pass_control.obj \
$(OBJDIR)\gpib_send_ifc.obj \
$(OBJDIR)\tclvisa.obj

DESTNAME=$(OUTDIR)\$(PACKAGE_NAME)$(PACKAGE_DLL_VERSION)
DEST=$(DESTNAME).dll

all: $(DEST)

$(DEST): $(OBJS)
	$(AR) /OUT:"$(DEST)" $(LDFLAGS) $(LIBS) $(CLIBS) $(OBJS)
# Uncomment following line to link manifect file (if any) to DLL
# /MT compiler switch makes it to use static linking without manifests
#	$(MT) -manifest "$(DEST).manifest" -outputresource:"$(DEST);2"

.SUFFIXES: .c .obj

{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	$(CC) $(CFLAGS) $< /Fo"$(OBJDIR)/"

$(OBJDIR):

clean:
	@del $(OBJS) "$(DESTNAME).exp" "$(DESTNAME).lib" "$(DESTNAME).dll.manifest" "$(OUTDIR)\vc*.idb" "$(OUTDIR)\vc*.pdb"

cleanall: clean
	@del "$(DEST)"

