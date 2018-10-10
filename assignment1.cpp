/*

Gonzalo Zepeda Galvan
ID: 1561524
COSC 3360
Due date: Februrary 23rd, 2018
Assignment #1: Process Scheduling
Description: To simulate the what core scheduling is like in a tablet by simulating the execution of processes.

*/

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

//each line of the input file is a single Scan 
struct Scan 
{ 
private:
	string scanType; 
	int scanTime;    
public:
	
	Scan(string x, int y)
	{
		scanType = x;
		scanTime = y;
	}
	string getScanType()
	{
		return scanType;
	}
	int getScanTime()
	{
		return scanTime;
	}
};

struct task
{ 
private:
	int taskID, arrivalTime;
	string status;
	vector<Scan> scans;
public:
	task(int loadID, int arrival)
	{
		taskID = loadID;
		arrivalTime = arrival;
	}
	void addScan(string scan, int scanTime)
	{
		Scan temp(scan, scanTime);
		scans.push_back(temp);
	}
	Scan getScan()
	{
		return scans[0];
	}
	void deleteScan()
	{
		scans.erase(scans.begin());
	}
	void setTaskNumber(int x)
        {
		taskID = x;
	}
	string getStatus()
	{
		return status;
	}
	int getTaskNumber()
	{
		return taskID;
	}
	void setStatus(string x)
	{
		status = x;
	}
	void setArrivalTime(int x)
	{
		arrivalTime = x;
	}
	int getArrivalTime()
	{
		return arrivalTime;
	}
	bool empty()
	{
		if (scans.size() == 0)
			return true;
		else return false;
	}
	void print()
	{
		cout << "process number: " << taskID << "  Arrival Time: " << arrivalTime << endl;
		for (unsigned int i = 0; i < scans.size(); i++)
		{
			cout << scans[i].getScanType() << " " << scans[i].getScanTime() << endl;;
		}
	}
};

//the required input device for the tablet
class deviceInput
{ 
private:
	int clockt;              // keeps track of how long device is busy
	int taskNumber;          // contains the current process number
	string status = "free";  
public:
	deviceInput()
	{
		clockt = 0;
	}
	void freeInput()
	{ 
		taskNumber = -1; //  no processes
		status = "free";
	}
	void setInputBusy(int x, int procNumber)
	{
		clockt = x;
		taskNumber = procNumber;
		status = "busy";
	}
	int getTaskID()
	{
		return taskNumber;
	}
	int getTimer()
	{
		return clockt;
	}
	bool isFree()
	{
		if (status == "free")
			return true;
		return false;
	}

};


//tablet required SSD, everything is the same as input so no documentation
class SSD
{ 
private:
	int clockt;
	int taskNumber;
	string status = "free";
public:
	SSD()
	{
		clockt = 0;
	}
	int getTaskID()
	{
		return taskNumber;
	}
	void freeSSD()
	{
		taskNumber = -1;
		status = "free";
	}
	void utilizeSSD(int x, int procNumber)
	{
		clockt = x;
		taskNumber = procNumber;
		status = "busy";
	}
	bool isFree()
	{
		if (status == "free")
			return true;
		return false;
	}
	int getTimer()
	{
		return clockt;
	}
};

class core
{
private:
	int coreID;        
	int taskNumber;
	int clockt;
	string status = "free";
public:
	core(int x)
	{
		coreID = x;
		clockt = 0;
	}
	void setCoreNum(int x, int procNumber)
	{
		coreID = x;
		taskNumber = procNumber;
	}
	void setCoreBusy(int x, int procNumber)
	{
		taskNumber = procNumber;
		clockt = x;
		status = "busy";
	}
	void setCoreFree()
	{
		taskNumber = -1;
		status = "free";
	}
	int getTaskID()
	{
		return taskNumber;
	}
	bool isFree()
	{
		if (status == "free")
			return true;
		return false;
	}
	int getTimer()
	{
		return clockt;
	}
};

class virtualTablet
{
private:

	int numberofCores, numberofTasks, tasksStarted = -1;
	// numberofCores = amount of cores the tablet has
	// numberofTasks = total number of processes
	// tasksStarted = how many processes have executed


	vector<core> coresVector; //vector contains all the cores
	vector<task> taskVector;
	SSD ssd;
	deviceInput inputD;
	queue <task> readyQueue; //contains the processes waiting for a core
	queue <task> ssdQueue;   //contains the processes waiting on the ssd
	queue <task> inputQueue; //contains the processes waiting on the input device
	queue <task> taskTable;
public:

	virtualTablet(int, int, vector<task>);
	void simulationSummary();
	void printStatusofCores();
	void printTasks(int);
	void eventLocator();
	int availableCores();
	void nextScan(task &);
	void coreScan(task&);
	void ssdScan(task&);
	void inputScan(task&);
	void taskArrival(task&);
	void coreExpiration(int);
	void ssdExpiration();
	void inputCompletion();
	void taskCompletion(int);

};




int clocktTime = 0;
double countSSD = 0;
double totalSSD = 0;
double coreUsage = 0;

virtualTablet::virtualTablet(int x, int y, vector<task> list)
{
	numberofTasks = x;
	numberofCores = y;
	for (int i = 0; i < numberofCores; i++)
	{ 
		core temp(i);
		coresVector.push_back(temp);
	}
	taskVector = list;
	eventLocator();
}
// function returns how many cores are free to use
int virtualTablet::availableCores()
{ 
	for (unsigned int i = 0; i < coresVector.size(); i++)
	{
		if (coresVector[i].isFree())
			return i;
	}
	return -1;
}
// function prints current core status
void virtualTablet::printStatusofCores()
{
	for (int i = 0; i < numberofCores; i++)
	{
		if (coresVector[i].isFree())
			cout << "core " << i << " is free" << endl;
		else
			cout << "core " << i << " is busy " << coresVector[i].getTimer() << endl;
	}
}
//function determines the next event by looking at the process table
void virtualTablet::eventLocator()
{ 
	int quickest = 99999999;
	int coreNUM;
	string event = "empty";
	task *temp;
	if (!taskTable.empty())
	{
		temp = &taskTable.front();
	}
	if (tasksStarted + 1 < numberofTasks)
	{
		if (taskVector[tasksStarted + 1].getArrivalTime() < quickest)
		{
			quickest = taskVector[tasksStarted + 1].getArrivalTime();
			event = "arrival";
		}
	}
	for (unsigned int i = 0; i < numberofCores; i++)
	{
		if (coresVector[i].isFree() == false)
		{
			if (coresVector[i].getTimer() < quickest)
			{
				quickest = coresVector[i].getTimer();
				event = "core";
				coreNUM = i;
			}
		}
	}
	if (!ssd.isFree())
	{
		if (ssd.getTimer() < quickest)
		{
			quickest = ssd.getTimer();
			event = "ssd";
		}
	}
	if (!inputD.isFree())
	{
		if (inputD.getTimer() < quickest)
		{
			quickest = inputD.getTimer();
			event = "input";
		}
	}
	if (event == "core")
	{
		clocktTime = quickest;
		coreExpiration(coreNUM);
	}
	else if (event == "ssd")
	{
		clocktTime = quickest;
		ssdExpiration();
	}
	else if (event == "input")
	{
		clocktTime = quickest;
		inputCompletion();
	}

	else if (event == "arrival")
	{
		clocktTime = quickest;
		taskArrival(taskVector[tasksStarted + 1]);
	}
	else
	{
		simulationSummary();
	}
}
// function is called after a scan is completed to move the process to its destination in the tablet
void virtualTablet::nextScan(task &temp)
{ 
	if (temp.empty())
	{
		taskCompletion(temp.getTaskNumber());
		eventLocator();
		return;
	}

	Scan next = temp.getScan();
	string scanType = next.getScanType();

	if (scanType == "CORE")
	{
		coreScan(temp);
    }
	else if (scanType == "SSD")
	{
		ssdScan(temp);
    }
	else if (scanType == "INPUT")
	{
		inputScan(temp);
    }
}
// function is called to initialize the cores
void virtualTablet::coreScan(task &temp)
{ 
	int coreNum = availableCores();
	Scan next = temp.getScan();
	int scanTime = next.getScanTime();

	cout << "-- Process " << temp.getTaskNumber() << " requests a core at time " << clocktTime << " ms for " << scanTime << " ms" << endl;
	if (coreNum != -1)
	{
		coresVector[coreNum].setCoreBusy(scanTime + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " will release a core at time  " << scanTime + clocktTime << endl;
		temp.setStatus("RUNNING");
	}
	else
	{
		cout << "-- Process " << temp.getTaskNumber() << " needs to wait for a free core" << endl;
		readyQueue.push(temp);
		temp.setStatus("READY");
		cout << "-- The Ready queue has " << readyQueue.size() << " processes waiting for a free core" << endl;
	}
	eventLocator();
}
// function is called to initialize the ssd
void virtualTablet::ssdScan(task &temp)
{ 
	Scan next = temp.getScan();
	int scanTime = next.getScanTime();

	cout << "-- Process " << temp.getTaskNumber() << " requests the ssd for: " << scanTime << " ms at " << clocktTime << " ms "<< endl;
	if (ssd.isFree())
	{
		ssd.utilizeSSD(scanTime + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " has the SSD for " << scanTime << endl;
	}
	else
	{
		cout << "-- Process " << temp.getTaskNumber() << " has been added to SSD queue for " << scanTime << endl;
		ssdQueue.push(temp);
	}
	temp.setStatus("BLOCKED");
	eventLocator();
}
// function is called to start tablet input
void virtualTablet::inputScan(task &temp)
{ 
	Scan next = temp.getScan();
	int scanTime = next.getScanTime();
	cout << "-- Process " << temp.getTaskNumber() << " requests the input device for: " << scanTime << " ms at " << clocktTime << endl;
	if (inputD.isFree())
	{
		inputD.setInputBusy(scanTime + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " starts input at " << clocktTime << endl;
	}
	else
	{
		inputQueue.push(temp);
		cout << "-- Process " << temp.getTaskNumber() << " has been added to input queue for " << scanTime << endl;
	}
	temp.setStatus("BLOCKED");
	eventLocator();
}
// function is called when a core is finished being utilized
void virtualTablet::coreExpiration(int coreNum)
{ 
	int taskFinished = coresVector[coreNum].getTaskID();
	Scan del = taskVector[taskFinished].getScan();
	coreUsage += del.getScanTime();
	taskVector[taskFinished].deleteScan();
	cout << "-- CORE completion event for process " << taskFinished << " at time " << clocktTime << " ms" << endl;
	if (readyQueue.empty())
	{
		coresVector[coreNum].setCoreFree();
		cout << "-- Core " << coreNum << " is now free" << endl;
		nextScan(taskVector[taskFinished]);
	}
	else
	{
		task temp = readyQueue.front();
		readyQueue.pop();
		Scan scan = temp.getScan();
		coresVector[coreNum].setCoreBusy(scan.getScanTime() + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " will release a core at time " << scan.getScanTime() + clocktTime << " ms " << endl;
		nextScan(taskVector[taskFinished]);
	}
}

// function is called when ssd is done being utilized
void virtualTablet::ssdExpiration()
{ 
	int taskFinished = ssd.getTaskID();
	Scan del = taskVector[taskFinished].getScan();
	countSSD += del.getScanTime();
	totalSSD++;
	taskVector[taskFinished].deleteScan();
	cout << "-- SSD completion event for process " << taskFinished << " at time " << clocktTime << " ms" << endl;
	if (ssdQueue.empty())
	{
		cout << "-- SSD is now free" << endl;
		ssd.freeSSD();
		nextScan(taskVector[taskFinished]);
	}
	else
	{
		task temp = ssdQueue.front();
		ssdQueue.pop();
		Scan scan = temp.getScan();
		ssd.utilizeSSD(scan.getScanTime() + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " will release SSD at time " << scan.getScanTime() + clocktTime << " ms " << endl;
		nextScan(taskVector[taskFinished]);
	}
}

// function is called when device is not reciving input
void virtualTablet::inputCompletion()
{ 
	int taskFinished = inputD.getTaskID();

	Scan del = taskVector[taskFinished].getScan();

	taskVector[taskFinished].deleteScan();
		cout << "-- CORE completion event for process " << taskFinished << " at time " << clocktTime << " ms" << endl;

	if (inputQueue.empty())
	{
		inputD.freeInput();
		cout << "-- Input device is free " << endl;
		nextScan(taskVector[taskFinished]);
	}
	else
	{
		task temp = inputQueue.front();
		inputQueue.pop();
		Scan scan = temp.getScan();
		inputD.setInputBusy(scan.getScanTime() + clocktTime, temp.getTaskNumber());
		cout << "-- Process " << temp.getTaskNumber() << " will release input device at time " << scan.getScanTime() + clocktTime << " ms " << endl;
		nextScan(taskVector[taskFinished]);
	}
}

void virtualTablet::taskArrival(task & temp)
{
	cout << "-- Arrival event for process " << temp.getTaskNumber() << " at time " << taskVector[temp.getTaskNumber()].getArrivalTime() << endl;
	clocktTime = taskVector[temp.getTaskNumber()].getArrivalTime();
	cout << endl;
	printTasks(tasksStarted);
	tasksStarted++;
	coreScan(temp);
}

// prints the status of each process once completed
void virtualTablet::taskCompletion(int x)
{ 
	cout << endl;
	cout << "Process " << x << " terminated at time " << clocktTime << " ms" << endl;
	taskVector[x].setStatus("Terminated");
	for (int i = 0; i < tasksStarted + 1; i++)
	{
		cout << "Process " << i << " is " << taskVector[i].getStatus() << endl;
	}
	cout << endl;
}

void virtualTablet::printTasks(int x)
{
	cout << "Process " << x + 1 << " starts at a time " << taskVector[x + 1].getArrivalTime() << " ms" << endl;
	if (x != -1)
	{
		for (int i = 0; i <= x; i++)
		{
			cout << "Process " << i << " is " << taskVector[i].getStatus() << endl;
		}
	}
	cout << endl;
}

// prints the summary
void virtualTablet::simulationSummary()
{ 
	cout << "SUMMARY " << endl;
	cout << "Number of processes completed: " << tasksStarted + 1 << endl;
	cout << "Total amount of SSD accesses: " << totalSSD << endl;
	cout << "Average SSD access time: ";
	printf("%.2f", (countSSD / totalSSD));
	cout << endl;
	cout << "Total time elapsed: " << clocktTime << " ms" << endl;
	cout << "Core Utilization: ";
	printf("%.2f", (coreUsage / clocktTime) * 100);
	cout << " percent" << endl;
	cout << "SSD Utilization: ";
	printf("%.2f", (countSSD / clocktTime) * 100);
	cout << " percent" << endl;
}

// main function of the program
int main()
{
	string word, fileName;
	int number;
	int taskCounter = -1;
	int coreCount;
	vector<task> taskVector;

	while (cin >> word && cin >> number)
	{
		if (word == "NCORES")
		{
			coreCount = number;
		}
		else if (word == "NEW")
		{
			taskCounter++;
			task temp(taskCounter, number);
			taskVector.push_back(temp);
		}
		else
		{
			taskVector.back().addScan(word,number);
		}
	}
	for (int i = 0; i <= taskCounter; i++)
	{
		taskVector[i].print();
		cout << endl;
	}
	taskCounter++;
	virtualTablet sys(taskCounter, coreCount, taskVector);
	return 0;
}


