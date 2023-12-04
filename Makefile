# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/02 19:06:48 by tlegrand          #+#    #+#              #
#    Updated: 2023/12/04 22:18:28 by tlegrand         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



#	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	PROJECT VARIABLES	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	#

#	==============================	NAMES	==============================	#
NAME		=	webserv
DIR			=	${DIR_SRCS_PARSE} ${DIR_SRCS_HTTP} ${DIR_SRCS_CLASS} 
DEFAULT_CONFIG_PATH	=	none

#	==============================	SOURCES	==============================	#
DIR_SRCS_PARSE	=	parsing/
LST_SRCS_PARSE	=	test.cpp
SRCS_PARSE		=	${addprefix ${DIR_SRCS_PARSE}, ${LST_SRCS_PARSE}}

DIR_SRCS_HTTP	=	http/
LST_SRCS_HTTP	=	testdep.cpp
SRCS_HTTP		=	${addprefix ${DIR_SRCS_HTTP}, ${LST_SRCS_HTTP}}

DIR_SRCS_CLASS	=	class/
LST_SRCS_CLASS	=	Request.cpp
SRCS_CLASS		=	${addprefix ${DIR_SRCS_CLASS}, ${LST_SRCS_CLASS}}

DIR_SRCS		=	src/
LST_SRCS		=	main.cpp \
					${SRCS_PARSE} ${SRCS_HTTP} ${SRCS_CLASS} 
SRCS			=	${addprefix ${DIR_SRCS}, ${LST_SRCS}}


#	==============================	OBJECTS	==============================	#
DIR_OBJS	=	.objs/

ifeq ($(DEBUG), fsanitize)
	LST_OBJS	=	${LST_SRCS:.cpp=.debug.o}
else ifeq ($(DEBUG), valgrind)
	LST_OBJS	=	${LST_SRCS:.cpp=.valg.o}
else
	LST_OBJS	=	${LST_SRCS:.cpp=.o}
endif

OBJS		=	${addprefix ${DIR_OBJS}, ${LST_OBJS}} 


#	==============================	DEPS	==============================	#
DIR_DEPS	=	.deps/
ifeq ($(DEBUG), fsanitize)
	DEPS	=	${addprefix ${DIR_OBJS}, ${LST_SRCS:.cpp=.debug.d}}
else ifeq ($(DEBUG), valgrind)
	DEPS	=	${addprefix ${DIR_OBJS}, ${LST_SRCS:.cpp=.valg.d}}
else
	DEPS	=	${addprefix ${DIR_OBJS}, ${LST_SRCS:.cpp=.d}}
endif

#	==============================	HEADERS	==============================	#
DIR_HEADER	=	inc/
LST_HDR		=	${NAME}.hpp Request.hpp
HEADER		=	${addprefix ${DIR_HEADER}, ${LST_HDR}}


#	==============================	COMMANDS	==============================	#
CXX			=	c++ -std=c++98
MKDIR 		=	mkdir -p
RM			=	rm -rf
MAKE		=	make --silent
DEBUG		=	no


#	==============================	FLAGS	==============================	#
CXXFLAGS	=	-Wall -Wextra  #-Werror
DEPFLAGS	=	-MMD -MP
FSFLAGS		=	-g3 -fsanitize=leak,address,pointer-subtract,pointer-compare,undefined 
VALFLAGS 	=	--leak-check=full --show-leak-kinds=all --track-origins=yes --show-mismatched-frees=yes \
				--track-fds=yes --trace-children=yes 
#				--time-stamp=yes --error-markers=begin,end
HARDFLAGS	=	--read-var-info=yes --read-inline-info=yes


ifeq ($(DEBUG), fsanitize)
	CXXFLAGS += $(FSFLAGS)
else ifeq ($(DEBUG), valgrind)
	CXXFLAGS += -g3
endif



#	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	RULES	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	#
.PHONY : all clean fclean re bonus FORCE nn

#	==============================	BASIC	==============================	#
all		:	${NAME}

clean	:
		@${RM} ${DIR_OBJS}

fclean	:	clean
		@${RM} ${NAME} 
		@printf "$(GREEN)All clean !\n$(END)"

re		:	fclean
		@${MAKE} all

fsan	:
	@${MAKE} DEBUG=fsanitize ${NAME} 

val		:
	@${MAKE} DEBUG=valgrind && valgrind ${VALFLAGS} ./${NAME} ${DEFAULT_CONFIG_PATH}

hard	:
	@${MAKE} DEBUG=valgrind && valgrind ${VALFLAGS} ${HARDFLAGS} ./${NAME} ${DEFAULT_CONFIG_PATH}



#	==============================	COMPILATION	==============================	#
${NAME}			:	${DIR_OBJS} ${OBJS}
				@${CXX} -I${DIR_HEADER} ${CXXFLAGS} ${OBJS} -o ${NAME}
				@printf "$(GREEN_LIGHT)$@ created !\n$(END)"


${DIR_OBJS}%.o	:	${DIR_SRCS}%.cpp 
				@printf "$(YELLOW)Making $@...\n$(END)"
				@${CXX} -I${DIR_HEADER} ${CXXFLAGS} ${DEPFLAGS} -c $< -o $@


${DIR_OBJS}%.debug.o	:	${DIR_SRCS}%.cpp 
				@printf "$(YELLOW)Making $@...\n$(END)"
				@${CXX} -I${DIR_HEADER} ${CXXFLAGS} ${DEPFLAGS} ${FSFLAGS} -c $< -o $@


${DIR_OBJS}%.valg.o	:	${DIR_SRCS}%.cpp 
				@printf "$(YELLOW)Making $@...\n$(END)"
				@${CXX} -I${DIR_HEADER} ${CXXFLAGS} ${DEPFLAGS} -c $< -o $@

-include $(DEPS)


#	==============================	UTILS/LIB	==============================	#
${DIR_OBJS}	:
			@${MKDIR} ${DIR_OBJS} ${addprefix ${DIR_OBJS}, ${DIR}}




#	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	OPERATING VARIABLES	/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\/*\	#

#	==============================	COLORS	==============================	#
BLACK		=	\033[0;30m
RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
BLUE		=	\033[0;34m
PURPLE		=	\033[0;35m
CYAN		=	\033[0;36m
GRAY_LIGHT	=	\033[0;37m
GRAY_DARK	=	\033[1;30m
RED_LIGHT	=	\033[1;31m
GREEN_LIGHT	=	\033[1;32m
YELLOW 		=	\033[1;33m
BLUE_LIGHT	=	\033[1;34m
VIOLET		=	\033[1;35m
CYAN		=	\033[1;36m
WHITE		=	\033[1;37m
END			=	\033[0m
BOLD		=	\033[1m
FAINT		=	\033[2m
ITALIC		=	\033[3m
UNDERLINE	=	\033[4m
BLINK_SLOW	=	\033[5m
BLINK_FAST	=	\033[6m
BLINK_OFF	=	\033[25m
REV_V		=	\033[7m
CONCEAL		=	\033[8m
CONCEAL_OFF	=	\033[28m
CROSS_OUT	=	\033[9m
CROSS_OUT_O	=	\033[29m
ERASE		=	\033[2K
RERASE		=	\r\033[2K
