include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libroseDisassemblers_la_SOURCES =											\
	SgAsmBlock.C SgAsmFunction.C											\
	SgAsmInstruction.C SgAsmArmInstruction.C SgAsmPowerpcInstruction.C SgAsmX86Instruction.C SgAsmMipsInstruction.C	\
	SgAsmInterpretation.C												\
	Expressions.C Partitioner.C PStatistics.C IPDParser.C IPDUnparser.C Registers.C					\
        Disassembler.C DisassemblerArm.C DisassemblerMips.C DisassemblerPowerpc.C DisassemblerX86.C			\
	Assembler.C AssemblerX86.C AssemblerX86Init.C									\
	AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C AssemblerX86Init4.C AssemblerX86Init5.C		\
	AssemblerX86Init6.C AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C
else
   libroseDisassemblers_la_SOURCES = dummyDisassembler.C
endif

include_HEADERS =											\
	Partitioner.h Registers.h									\
	Disassembler.h DisassemblerArm.h DisassemblerMips.h DisassemblerPowerpc.h DisassemblerX86.h	\
	Assembler.h AssemblerX86.h AssemblerX86Init.h							\
	InstructionEnumsX86.h InstructionEnumsMips.h

dist_include_HEADERS =

EXTRA_DIST = CMakeLists.txt dummyDisassembler.C
