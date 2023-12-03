# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/03 13:07:36 by tlegrand          #+#    #+#              #
#    Updated: 2023/12/03 13:08:13 by tlegrand         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



COMPOSE	=	docker-compose -f compose.yml 

all		:	--msg
	

--msg	:
	@echo "pas finie... :)"


clean	:
		${COMPOSE} down --rmi all -v --remove-orphans

fclean	:	clean

re	:	fclean all


up 	:	
	${COMPOSE} up -d 
	
down:
	${COMPOSE} down 
	
top:
	${COMPOSE} top

logs:
	@echo "			**** LOGS ****"
	@echo "=========== nginx ================"
	@docker logs nginx


ps :
	@${COMPOSE} ps


.PHONY:	all re clean fclean up down prune ps data volumes

# docker stop $(docker ps -qa); docker rm $(docker ps -qa); docker rmi -f $(docker images -qa); docker volume rm $(docker volume ls -q); docker network rm $(docker network ls -q);
