#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <exception>
#include <Windows.h>

using namespace std;

constexpr auto IND = 31;
constexpr auto TAB = 4;
mutex m;

void posXY(const int column, const int line) {
	COORD coord;
	coord.X = column;
	coord.Y = line;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, coord);
}

void printBar(const int serialNumber, const int calcDurat) {
	auto start = chrono::high_resolution_clock::now();
	m.lock();
	posXY(0, serialNumber);
	cout << "Thread " << serialNumber << " with thread id " << this_thread::get_id() << '\t';
	m.unlock();
	this_thread::yield();
	for (int progress = 0; progress <= calcDurat; ++progress) {
		this_thread::sleep_for(chrono::milliseconds(100));
		m.lock();
		posXY((IND + progress), serialNumber);
		cout << "||";
		m.unlock();
		this_thread::yield();
	}
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::ratio<1, 1000>> time = end - start;
	lock_guard<mutex> lg(m);
	posXY((IND + calcDurat + TAB), serialNumber);
	cout << time.count() << " ms";
}

int main() {
	try {
		int thrQuant = 4;
		int calcDurat = 30;
		if (thrQuant <= 0 || calcDurat <= 0) {
			throw runtime_error("Wrong inputs");
		}
		else {
			vector<thread> threads;
			for (int i = 0; i < thrQuant; ++i)
				threads.push_back(thread(printBar, i, calcDurat));
			for (auto& t : threads)
				t.join();

			for (int i = 0; i < thrQuant; ++i)
				cout << '\n';
		}
	}
	catch (std::runtime_error& e) {
		cout << e.what() << '\n';
	}
	catch (...) {
		cout << "Something has gone wrong!" << '\n';
	}

	return 0;
}