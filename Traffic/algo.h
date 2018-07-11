#include <iostream>
#include<fstream>
#include<stdio.h>
#include<ctime>
#include<stdlib.h>
#include<windows.h>
#include<dos.h>
#define threshold_2 10
using namespace std;
class junction
{
public:
	int ip_data[4];
	float relative_factor[4];
	int previous_data[4];
	int current_data[4];
	int id;
	int signal[4];
	float factor;
	int time_signal[4];
	int t_signal[4];
	int extra_time[4];
	float threshold;
	const int fixed_time = 240;
	void read_ip_data(int,int);
	void find_extra_time();
	void calculate_ip_time();
	int find_relative_factor();
	void fix_min_ratio();
	void signal_generation();
	void recieve_previous_data();
	void display();
};
void junction::read_ip_data(int data1,int data2)
{
	int i, sum = 0, return_flag;
	ip_data[0] = data1;
	ip_data[1] = rand() % 25;
	ip_data[2] = data2;
	ip_data[3] = rand() % 25;
	threshold = 0.125;
	cout << "Collected IP data" << endl;
	for (int j = 0; j < 4; j++)
		cout << ip_data[j] << endl;
	find_relative_factor();
}
void junction::signal_generation()
{
	int j, z = 0, max_time_signal = 0;
	int i, extra_t = 0;
	for (i = 0; i<4; i++)
		extra_t += extra_time[i];
	int max_time = fixed_time - extra_t;
	//cout << max_time;
	for (i = 0; i<4; i++)//based on relative factor,timr allocations fot signals
		time_signal[i] = relative_factor[i] * max_time;
	for (i = 0; i<4; i++)//calculating which lane has got greater time
	{
		for (j = i + 1; j<4; j++)
		{
			if (time_signal[i]>time_signal[j])
				max_time_signal = i;
			else
				max_time_signal = j;
		}
	}
	for (j = 0; j<4; j++)
		z += time_signal[j];
	for (i = 0; i<4; i++)
	{
		if (i == max_time_signal)//adding integer differnce to greater time
		{
			time_signal[i] += max_time - z;
			signal[i] = 0;
			for (int j = 0; j<i; j++)
				signal[i] += time_signal[j];
		}
		else if (i == 0)//first lane signal display time
		{
			signal[0] = time_signal[0];
		}
		else//rest signal display time
		{
			signal[i] = 0;
			for (int j = 0; j<i; j++)
				signal[i] += time_signal[j];
		}
	}

}
void junction::display()
{

	for (int j = 0; j<4; j++)
	{
		t_signal[j] = signal[j] + extra_time[j];
	}


	ofstream myfile;
	myfile.open("Output.txt", ios::app);
	time_t now = time(0);
	char *dt = ctime(&now);
	myfile << dt;
	for (int k = 0; k<4; k++)
		myfile << k + 1 << " lane=" << time_signal[k] << " seconds" << "\n";
	myfile << endl << endl;
	myfile.close();
	int i = 0;

	while (i<4)
	{
		cout << "\nGREEN FOR WAY" << i + 1 << endl;
		printf(" WAY1  WAY2  WAY3  WAY4\n");


		for (int j = 0; j<4; j++)
		{
			t_signal[j] = signal[j];
		}

		for (int q = 0; q<time_signal[i]; q++)
		{
			if (i == 0) // green signal
			{
				printf("%5d- %5d %5d %5d\n", t_signal[0]--, t_signal[1]--, t_signal[2]--, t_signal[3]--);
				Sleep(200);
			}
			else if (i == 1) // next green
			{
				printf("%5d %5d- %5d %5d\n", t_signal[0]--, t_signal[1]--, t_signal[2]--, t_signal[3]--);
				Sleep(200);
			}
			else if (i == 2) // next next green
			{
				printf("%5d %5d %5d- %5d\n", t_signal[0]--, t_signal[1]--, t_signal[2]--, t_signal[3]--);
				Sleep(200);
			}
			else // next next next green
			{
				printf("%5d %5d %5d %5d-\n", t_signal[0]--, t_signal[1]--, t_signal[2]--, t_signal[3]--);
				Sleep(00);
			}
		}

		signal[i % 4] = 220 - time_signal[i];//lane which had green
		signal[(i + 1) % 4] = time_signal[(i + 1) % 4];//lane which is having next green
		signal[(i + 2) % 4] = signal[(i + 2) % 4] - time_signal[i];
		signal[(i + 3) % 4] = signal[(i + 3) % 4] - time_signal[i];
		i++;
	}
}
int junction::find_relative_factor()
{
	int i, sum = 0, return_flag;
	if (ip_data[0]>5 || ip_data[1]>5 || ip_data[2]>5 || ip_data[3]>5)
	{
		for (i = 0; i<4; i++)
			sum += ip_data[i];
		for (i = 0; i<4; i++)
			relative_factor[i] = (float)ip_data[i] / sum;
		return_flag = 1;
	}
	else
	{

		for (i = 0; i<4; i++)
			relative_factor[i] = 0.25;
		return_flag = 0;
	}
	if (return_flag = 1)
	{
		fix_min_ratio();
		return return_flag;
	}
	return return_flag;
}
void junction::fix_min_ratio()
{
	int min_cnt = 0;
	//  const float threshold=0.125;
	int i;
	bool flag[4] = { 0 }, flag_check = false;
	for (i = 0; i<4; i++)
		if (relative_factor[i]<threshold)
		{
			flag[i] = 1;
			relative_factor[i] = threshold;
			flag_check = true;
		}
	if (flag_check)
	{
		float allocated_ratio = 0, ratio_sum = 0;
		for (i = 0; i<4; i++)
			if (flag[i])
				allocated_ratio += relative_factor[i];
			else
				ratio_sum += relative_factor[i];
		float remaining_ratio = 1 - allocated_ratio;
		for (i = 0; i<4; i++)
			if (flag[i] == false)
				relative_factor[i] = (relative_factor[i] * remaining_ratio) / ratio_sum;
	}
}
void junction::find_extra_time()
{
	int flag[4] = { 0 }, current_sum = 0, flag_check = 0;
	int i;
	float current_rel_ratio[4] = { 0.0 };
	//cout << "Enter data from previous signals" << endl;
	//cin >> previous_data[0] >> previous_data[1] >> previous_data[2] >> previous_data[3];
	//cout << "Sending these data as current data " << endl;
	previous_data[0] = 10;
	previous_data[1] = 8;
	previous_data[2] = 15;
	previous_data[3] = 23;

	for (i = 0; i<4; i++)
		current_data[i] = previous_data[i];
	for (i = 0; i<4; i++)
		if (current_data[i]>threshold_2)
		{
			flag[i] = 1;
			flag_check = 1;
		}
	if (flag_check == 1)
	{
		for (i = 0; i<4; i++)
			current_sum += current_data[i];
	//	cout << current_sum << endl;
		for (i = 0; i<4; i++)
			current_rel_ratio[i] = (float)current_data[i] / current_sum;
	}
	for (i = 0; i<4; i++)
	{
		if (flag[i] == 1 && current_rel_ratio[i]>0.25)
		{
			if (current_rel_ratio[i]>.5)
				extra_time[i] = 20;
			else if (current_rel_ratio[i]>.70)
				extra_time[i] = 30;
			else
				extra_time[i] = 10;
		}
	}
	/*for (i = 0; i<4; i++)
	{
		cout << extra_time[i] << endl;
	}*/
}
junction j;
void second_main(int data1,int data2)
{
	//int i=10;
	//int data = 10;
	// while(i--)
	{
		j.read_ip_data(data1,data2);
		j.find_extra_time();
		j.signal_generation();
		j.display();
	}
}
