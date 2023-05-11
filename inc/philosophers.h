/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfouadi <mfouadi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 19:29:07 by mfouadi           #+#    #+#             */
/*   Updated: 2023/05/11 02:21:41 by mfouadi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H
#define ARGUMENTS_PROTOTYPE "number_of_philosophers time_to_die time_to_eat \
time_to_sleep [number_of_times_each_philosopher_must_eat] (The last argument \
is optional)"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <stdbool.h>

typedef enum t_states
{
	EATING,
	SLEEPING,
	THINKING
}	s_states;

typedef struct s_philo
{
	pthread_t				tid;
	int						philo_id;
	int						dead_or_alive; // 1 - alive ; 0 - dead
	int						curr_state;
	int						last_meal;
	int						my_fork;// 0 - doesn't have a fork ; 1 - has a fork
	struct s_philo			*next;
	struct s_philo			*prev;
}	t_philo;

typedef struct	s_data
{
	t_philo			*philos;
	pthread_mutex_t	mtx;
	bool			simulation;
	int				nbr_of_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_meals; // -1 means, eat until simulation eventually ends
}	t_data;

int		init_simulation(t_data *data, char **av);
void	free_ptr(pthread_t **p);
int		ft_atoi_parser(char *s);
size_t	ft_strlen(char *s);
int		print_err(char *s, int exit_status);
bool	ft_isdigit(int c);
char	*ft_strdup(const char *s1);


t_philo *create_and_init_philo(int content);
int		insert_back(t_philo **head, t_philo *new_node);
void	clear_list(t_philo *head, int list_size);


#endif // PHILOSOPHERS_H