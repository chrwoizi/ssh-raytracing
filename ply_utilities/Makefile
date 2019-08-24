############;-*-fundamental-*-###############################################
# 
#               Makefile for PLY library and simple applications
#               ================================================
#
#############################################################################


# These are the names of various programs used during "making".
# Pick whichever flavor of C you like with the CC variable.

CC	= gcc
RM	= rm -f
CO	= co
YACC    = yacc
LEX     = lex

# INCLUDE supplies paths to header files.
INCLUDE		= -I. 

# optimization flags 
OPTIMIZE	= -O3
#OPTIMIZE       = -g

# DBFLAGS contains flags for debugging code
#DBFLAGS         = -pg
DBFLAGS         =

# CFLAGS is all of the compiler options.  Put any extras here.
CFLAGS		=  $(OPTIMIZE) $(INCLUDE) $(DBFLAGS) #-Wall

# These are the libraries linked to the program.  example: -lm
LIBS		= -lm 
#-lmalloc

#------------------------------ Lists of Files -------------------------------#

#The compiled library
LIB = libply.a

#--------------------------------- Main Rules --------------------------------#

#IMPORTANT - the object files must come BEFORE the libraries!!!

all: $(LIB)

libply.a: plyfile.c ply.h
	$(CC) $(CFLAGS) -o plyfile.o -c plyfile.c
	ar cruv libply.a plyfile.o 
	-ranlib libply.a

# Remove any intermediate files.
clean :
	@-$(RM) *.o *~ *% $(LIB)

.c : 
# AFS on HPs won't overwrite executable, so remove first
	@$(RM) $@    
	$(CC) $(CFLAGS) -o $@ $@.c $(LIB) $(LIBS)

