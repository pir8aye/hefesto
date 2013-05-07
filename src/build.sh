#
# Hefesto's bootstrap :)
#

COMPILER="gcc" # Adjust for your system compiler
COMPILER_OPTS="-c " # Options too
LINKER="gcc" # Linker too
LINKER_OPTS="-o../bin/hefesto dbg.o dep_chain.o expr_handler.o exprchk.o file_io.o hlsc_msg.o\
                htask.o hvm.o hvm_alu.o hvm_func.o hvm_list.o hvm_rqueue.o hvm_str.o hvm_syscall.o\
                     hvm_thread.o hvm_toolset.o init.o lang_defs.o main.o mem.o os_detect.o parser.o\
                         regex.o src_chsum.o structs_io.o synchk.o types.o vfs.o hvm_project.o -lpthread"

UNIT_TEST="-omain ../../dbg.o ../../dep_chain.o ../../expr_handler.o ../../exprchk.o ../../file_io.o ../../hlsc_msg.o\
            ../../htask.o ../../hvm.o ../../hvm_alu.o ../../hvm_func.o ../../hvm_list.o ../../hvm_rqueue.o ../../hvm_str.o\
                ../../hvm_syscall.o ../../hvm_thread.o ../../hvm_toolset.o ../../init.o ../../lang_defs.o ../../mem.o ../../os_detect.o\
                  ../../parser.o ../../regex.o ../../src_chsum.o ../../structs_io.o ../../synchk.o ../../types.o ../../vfs.o main.o htest.o ../../hvm_project.o -lpthread"

ALL_OK=1

# I know... this is ugly... boo!

echo "### Compiling..."

$COMPILER $COMPILER_OPTS dbg.c
if test $? -gt 0
then
ALL_OK=0
fi
$COMPILER $COMPILER_OPTS dep_chain.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS expr_handler.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS exprchk.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS file_io.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hlsc_msg.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS htask.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_alu.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_func.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_list.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_rqueue.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_str.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_syscall.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_thread.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_toolset.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS init.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS lang_defs.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS main.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS mem.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS os_detect.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS parser.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS regex.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS src_chsum.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS structs_io.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS synchk.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS types.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS vfs.c
if test $? -gt 0
then
    ALL_OK=0
fi
$COMPILER $COMPILER_OPTS hvm_project.c
if test $? -gt 0
then
    ALL_OK=0
fi

if test $ALL_OK -eq 1
then
echo "### Compiled."

echo "### Now running unit tests"

# Running unit tests

cd tests/unit
$COMPILER -c main.c
$COMPILER -c htest.c
$LINKER $UNIT_TEST
./main

if test $? -gt 0
then
    ALL_OK=0
fi

cd ../../

if test $ALL_OK -eq 1
then
    echo "### Unit tests are ok."
    # Now linking
    echo "### Linking..."
    $LINKER $LINKER_OPTS

    if test $? -eq 0
    then
        echo "### Linked."

        # If tests are ok, use the hefesto to install hefesto ;)

        ../bin/hefesto --forgefiles=../setup/hfst-inst.hls --hfst-inst-projects=hefesto-install

    fi
    # Done!
else
    echo "### Unit tests has errors!"
fi

else
    echo "### Compilation errors!!"
fi
