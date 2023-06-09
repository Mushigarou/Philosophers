/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfouadi <mfouadi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 19:26:45 by mfouadi           #+#    #+#             */
/*   Updated: 2023/05/30 01:17:00 by mfouadi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philosophers.h>
#define MAX_THREADS 2100
// ./philo 5 800 200 200 no one sould die (285123 ms)
// ./philo 4 410 200 200 (346800 ms)
// ./philo 5 310 200 200 (die at time)
// ./philo 2 310 200 200 (consistent delay of 1 - 4ms)
// .philo 5 800 200 200 7

//	if philo id is in an odd place, he wait time_to_eat. else, if he is
// in an even position they quit function
void	synchronize_access(t_philo *philo)
{
	unsigned long	start_time;

	if (philo->philo_id % 2 != 0)
	{
		start_time = get_time_in_ms(philo->start);
		if ((philo->philo_id % 2 != 0) && (philo->data->nbr_of_philos > 1))
		{
			pthread_mutex_lock(&philo->death_mtx);
			while (((get_time_in_ms(philo->start) - start_time)
					< philo->data->time_to_eat)
				&& (philo->status == ALIVE))
			{
				pthread_mutex_unlock(&philo->death_mtx);
				usleep(100);
				pthread_mutex_lock(&philo->death_mtx);
			}
		}
		pthread_mutex_unlock(&philo->death_mtx);
	}
	return ;
}

//	Init mutexes then entering philosophers life routine. in case something
//	happenned destroy mutexes
static void	dining_philosophers_routine(t_philo *philo)
{
	pthread_mutex_init(&philo->mtx, NULL);
	pthread_mutex_init(&philo->death_mtx, NULL);
	synchronize_access(philo);
	while (1)
	{
		pthread_mutex_lock(&philo->death_mtx);
		if (philo->status == DEAD)
			break ;
		pthread_mutex_unlock(&philo->death_mtx);
		if (pick_forks(philo) == -1)
			break ;
		eat(philo);
		put_forks(philo);
		pthread_mutex_lock(&philo->death_mtx);
		if (philo->status == DEAD)
			break ;
		pthread_mutex_unlock(&philo->death_mtx);
		ft_sleep(philo);
		if (philo->nbr_of_meals_taken == philo->data->number_of_meals)
			break ;
	}
	return ;
}

//	Init mutexes and create threads
int	start_simulation(t_data *data, t_philo philo[], struct timeval *start)
{
	unsigned long	i;

	data->simulation = 1;
	pthread_mutex_init(&data->print_mtx, NULL);
	pthread_mutex_init(&data->data_mtx, NULL);
	gettimeofday(start, NULL);
	i = -1;
	while (++i < data->nbr_of_philos)
	{
		init_philo_data(&philo[i], data, i);
		philo[i].start = *start;
		philo[i].next = &philo[((i + 1) % data->nbr_of_philos)];
		philo[i].prev = &philo[((i - 1 + data->nbr_of_philos)
				% data->nbr_of_philos)];
		if (pthread_create(&philo[i].tid, NULL,
				(void *)dining_philosophers_routine, &philo[i]) != 0)
			return (2);
	}
	return (0);
}

// printf death of a philo if it happenned, and destroy mutexes
void	end_simulation(t_data *data, pthread_t grim)
{
	unsigned long	i;

	pthread_join(grim, NULL);
	if (data->death_id != 0)
		printf("%lu	philo %d has died\n", get_time_in_ms(data->start),
			data->death_id);
	i = -1;
	while (++i < data->nbr_of_philos)
	{	
		pthread_mutex_destroy(&data->philo_head->death_mtx);
		pthread_mutex_destroy(&data->philo_head->mtx);
		data->philo_head = data->philo_head->next;
	}
	pthread_mutex_destroy(&data->print_mtx);
	pthread_mutex_destroy(&data->data_mtx);
	return ;
}

int	main(int ac, char **av)
{
	struct timeval	start;
	static t_philo	philo[MAX_THREADS];
	t_data			data;
	pthread_t		grim;
	unsigned long	i;

	if (ac <= 4 || ac > 6)
		return (print_err(PROGRAM_ARGUMENTS_PROTOTYPE, 1));
	if (init_data_struct(&data, av) == -1)
		return (print_err(ARGUMENT_ERROR, 1));
	if (start_simulation(&data, philo, &start) != 0)
		return (2);
	data.philo_head = &philo[0];
	data.start = start;
	if (pthread_create(&grim, NULL, (void *)grim_reaper, &data))
		return (2);
	i = -1;
	while (++i < data.nbr_of_philos)
		if (pthread_join(philo[i].tid, NULL) != 0)
			return (write(2, "pthread_join", ft_strlen("pthread_join")), 2);
	// pthread_mutex_lock(&data.data_mtx);
	data.simulation = 0;
	// pthread_mutex_unlock(&data.data_mtx);
	end_simulation(&data, grim);
	return (0);
}
