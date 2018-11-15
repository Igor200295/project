#include <iostream>
#include <iomanip>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;

#define PI 3.14159265358979323846264338327950288419716939937510

// Число разбиений исходного отрезка
static long num_steps = 10;
double I;
double a, b;
// Аналитическое значение интеграла
double I_real = 2.0 + PI;

/// Функция f(x) вычисляет значение по формуле f(x) = 4.0 / (1 + x^2)^2
double f(double x)
{
	double y = x;
	y *= y; // y = x^2
	y += 1; // y = 1 + x^2
	y *= y; // y = (1 + x^2)^2
	return 4.0 / y;
}

/// Функция CalculateIntegral() вычисляет значение интеграла от a до b от функции
/// f(x) по формуле Симпсона, num_steps - число разбиений отрезка [a, b]
double CalculateIntegral(double f(double), double a, double b, long num_steps)
{
	double step = (b - a) / num_steps;
	cilk::reducer_opadd<double> sum;
	cilk_for(int i = 0; i < num_steps; ++i)
	{
		double x1 = a + i*step;
		double x2 = a + (i + 0.5)*step;
		double x3 = a + (i + 1)*step;
		sum += (f(x1) + 4.0*f(x2) + f(x3));
	}
	return sum->get_value() * step / 6.0;
}

int main()
{
	a = -1.0;
	b = 1.0;

	std::cout.precision(20);
	for (int n = 0; n < 7; ++n)
	{
		std::cout << "Number of steps is " << num_steps << std::endl;
		high_resolution_clock::time_point t_start = high_resolution_clock::now();
		I = CalculateIntegral(f, a, b, num_steps);
		high_resolution_clock::time_point t_end = high_resolution_clock::now();
		duration<double> duration = (t_end - t_start);
		std::cout << "I = " << I << std::endl;
		std::cout << "abs(I - I_real) = " << std::abs(I - I_real) << std::endl;
		std::cout << "Time of calculation is " << duration.count() << " sec" << std::endl;
		std::cout << std::endl;
		num_steps *= 10;
	}

	return 0;
}