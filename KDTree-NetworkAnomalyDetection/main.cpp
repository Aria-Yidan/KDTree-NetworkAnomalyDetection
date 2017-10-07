#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>

using namespace std;


typedef struct dataNode
{
	float *states;
	struct dataNode *next;
}*DATALIST;

typedef struct kdNode
{
	int dividestate;	// 22-31, from 0 to 9
	float divideval;	// (max + min) / 2
	struct kdNode *leftchild;
	struct kdNode *rightchild;
	DATALIST points;	// decide leaf or mid
}*KDTree;

void changetoend(char** data, int index, int num)
{
	char temp[256] = { 0 };
	strcpy(temp, data[index]);
	memset(data[index], 0, sizeof(data[index]));
	strcpy(data[index], data[num]);
	memset(data[num], 0, sizeof(data[num]));
	strcpy(data[num], temp);
}

void RawDataProcess(char* rawfile, float* maxstate, float* minstate)
{
	int i = 0, j = 0, index = 0, count = 0, nownum = 0;
	int num1 = 0, num2 = 0, statenum = 10;
	float tempnum = 0.0;
	char *substr = NULL;
	char str[256] = { 0 }, tempstr[100] = { 0 };
	char normalfile[] = "nomal.txt", dosfile[] = "dos.txt";
	char trainfile[] = "train.txt", testfile[] = "test.txt";
	char **normaldata = NULL, **dosdata = NULL;
	char **traindata = NULL, **testdata = NULL;

	/*	Rawdata Divide */
	ifstream rawdata(rawfile);
	ofstream normalout(normalfile);
	ofstream dosout(dosfile);
	while (!rawdata.eof())
	{
		rawdata.getline(str, sizeof(str));
		j = (int)(strlen(str) - 1);
		while (str[j] != ',')
		{
			j--;
		}
		memcpy(tempstr, str + j + 1, strlen(str) - 1 - j);

		if (strcmp(tempstr, "normal.") == 0)
		{
			normalout << str << endl;
			num1++;
		}
		else if (strcmp(tempstr, "land.") == 0 || strcmp(tempstr, "neptune.") == 0
			|| strcmp(tempstr, "teardrop.") == 0 || strcmp(tempstr, "smurf.") == 0)
		{
			dosout << str << endl;
			num2++;
		}

		/* compute each state's max and min */
		count = 0;
		substr = strtok(str, ",");
		while (substr)
		{
			if (count >= 21 && count <= 30)
			{
				tempnum = (float)atof(substr);
				if (maxstate[count - 21] == -1.0)
				{
					maxstate[count - 21] = tempnum;
				}
				else if (maxstate[count - 21] < tempnum)
				{
					maxstate[count - 21] = tempnum;
				}
				if (minstate[count - 21] == -1.0)
				{
					minstate[count - 21] = tempnum;
				}
				else if (minstate[count - 21] > tempnum)
				{
					minstate[count - 21] = tempnum;
				}
			}
			substr = strtok(NULL, ",");
			count++;
		}

		memset(str, 0, 256);
		memset(tempstr, 0, 100);
		i++;
	}
	rawdata.close();
	normalout.close();
	dosout.close();

	cout << "raw : " << i << endl;
	cout << "normal : " << num1 << endl;
	cout << "dos : " << num2 << endl;

	/*	Init Traindata	*/
	i = 0;
	memset(str, 0, 256);
	normaldata = new char*[num1];
	ifstream normalin(normalfile);
	while (!normalin.eof() && i < num1)
	{
		normalin.getline(str, sizeof(str));
		normaldata[i] = new char[256];	//	for change
		memset(normaldata[i], 0, 256);
		strcpy(normaldata[i], str);
		memset(str, 0, 256);
		i++;
	}
	normalin.close();

	srand(unsigned(time(0)));
	int *normaluse = new int[num1];
	traindata = new char*[(int)(num1*0.7)];
	for (i = 0; i < num1; i++)
	{
		normaluse[i] = 0;
	}

	index = 0;
	nownum = num1;
	for (i = 0; i < (int)(num1 * 0.7); i++)
	{
		index = rand() % (nownum);
		traindata[i] = new char[strlen(normaldata[index])];
		strcpy(traindata[i], normaldata[index]);
		changetoend(normaldata, index, nownum - 1);
		normaluse[nownum - 1] = 1;
		nownum--;
	}

	/*	Init Testdata	*/
	testdata = new char*[(num1 - (int)(num1*0.7)) * 2];		// normal : dos = 1 : 1
	i = 0;
	memset(str, 0, 256);
	dosdata = new char*[num2];
	ifstream dosin(dosfile);
	while (!dosin.eof() && i < num2)
	{
		dosin.getline(str, sizeof(str));
		dosdata[i] = new char[256];		// for change
		memset(dosdata[i], 0, 256);
		strcpy(dosdata[i], str);
		memset(str, 0, 256);
		i++;
	}
	dosin.close();

	j = 0;
	for (i = 0; i < num1; i++)
	{
		if (normaluse[i] == 0)
		{
			testdata[j] = new char[strlen(normaldata[i])];
			strcpy(testdata[j], normaldata[i]);
			j++;
		}
	}

	int *dosuse = new int[num2];
	for (i = 0; i < num2; i++)
	{
		dosuse[i] = 0;
	}
	index = 0;
	nownum = num2;
	for (i = j; i < (num1 - (int)(num1*0.7)) * 2; i++)
	{
		index = rand() % (nownum);
		testdata[i] = new char[strlen(dosdata[index])];
		strcpy(testdata[i], dosdata[index]);
		changetoend(dosdata, index, nownum - 1);
		dosuse[nownum - 1] = 1;
		nownum--;
	}

	/*	free()...	*/
	for (i = 0; i < num1; i++)
	{
	delete[] normaldata[i];
	}
	delete normaldata;
	for (i = 0; i < num2; i++)
	{
	delete[] dosdata[i];
	}
	delete dosdata;
	delete[] normaluse;
	delete[] dosuse;
	

	/*  Write train.txt */
	ofstream trainout(trainfile);
	for (i = 0; i < (int)(num1 * 0.7); i++)
	{
		trainout << traindata[i];
		if (i + 1 < (int)(num1 * 0.7))
		{
			trainout << endl;
		}
	}
	trainout.close();

	/*  Write test.txt */
	ofstream testout(testfile);
	for (i = 0; i < (num1 - (int)(num1 * 0.7)) * 2; i++) {
		testout << testdata[i];
		if (i + 1 < (num1 - (int)(num1 * 0.7)) * 2) {
			testout << endl;
		}
	}
	testout.close();

	/* free...	*/
	delete[] traindata;
	delete[] testdata;
}

DATALIST TrainDataStd(char* trainfile, char* trainstdfile, float* maxstate, float* minstate)
{
	float min = 0.0, max = 1.0;		// new_min, new_max
	int statenum = 10;  // 22-31
	int i = 0, count = 0;
	char str[256] = { 0 };
	char *tempstr = NULL;

	DATALIST nowstd = NULL;
	DATALIST trainstd = new dataNode;
	trainstd->states = NULL;
	trainstd->next = NULL;

	/*	Read train.txt	*/
	float tempnum = 0.0;
	ifstream trainin(trainfile);
	nowstd = trainstd;
	while (!trainin.eof())
	{
		trainin.getline(str, sizeof(str));

		/* get states */
		count = 0;
		DATALIST tempstd = new dataNode;
		tempstr = strtok(str, ",");
		tempstd->states = new float[statenum];
		while (tempstr)
		{
			if (count >= 21 && count <= 30)
			{
				tempnum = (float)atof(tempstr);
				tempstd->states[count - 21] = tempnum;
			}
			tempstr = strtok(NULL, ",");
			count++;
		}

		tempstd->next = NULL;
		nowstd->next = tempstd;
		nowstd = tempstd;
		memset(str, 0, sizeof(str));
	}

	/*  Std */
	nowstd = trainstd->next;
	while (nowstd)
	{
		for (i = 0; i < statenum; i++)
		{
			if (maxstate[i] == minstate[i])	// special case
			{
				nowstd->states[i] = min;
			}
			else
			{
				nowstd->states[i] = (nowstd->states[i] - minstate[i]) * (max - min) / (maxstate[i] - minstate[i]) + min;
			}
		}
		nowstd = nowstd->next;
	}

	/*	Delete repetition	*/
	nowstd = trainstd->next;
	DATALIST prestd = NULL, tempstd = NULL;
	while (nowstd)
	{
		prestd = nowstd;
		tempstd = nowstd->next;
		while (tempstd)
		{
			for (i = 0; i < statenum; i++)
			{
				if (nowstd->states[i] != tempstd->states[i])
				{
					break;
				}
			}
			if (i == statenum)	// find repetition
			{
				prestd->next = tempstd->next;
			}
			else
			{
				prestd = tempstd;
			}
			tempstd = tempstd->next;
		}
		nowstd = nowstd->next;
	}

	/*  Write trainstd.txt  */
	ofstream trainstdout(trainstdfile);
	nowstd = trainstd->next;
	memset(str, 0, sizeof(str));
	while (nowstd)
	{
		for (i = 0; i < statenum; i++)
		{
			sprintf(str, "%f", nowstd->states[i]);
			trainstdout << str << ' ';
			memset(str, 0, sizeof(str));
		}
		if (nowstd->next)
		{
			trainstdout << endl;
		}
		nowstd = nowstd->next;
	}
	trainstdout.close();

	/*	free()	*/

	return trainstd;
}

void BuidKDTree(DATALIST trainstd, int profile, int pointnum, KDTree root)
{
	if (pointnum > profile)
	{
		int i = 0, tempstate_index = 0;
		float tempsub = 0.0;
		float tempstate_max[10] = { 0.0 }, tempstate_min[10] = { 0.0 };		// 22-31
		DATALIST nowstd = trainstd->next;

		/*	Find divid_state */
		if (nowstd)
		{
			for (i = 0; i < 10; i++)
			{
				tempstate_max[i] = nowstd->states[i];
				tempstate_min[i] = nowstd->states[i];
			}
		}
		while (nowstd)
		{
			for (i = 0; i < 10; i++)
			{
				if (tempstate_max[i] < nowstd->states[i])
				{
					tempstate_max[i] = nowstd->states[i];
				}
				if (tempstate_min[i] > nowstd->states[i])
				{
					tempstate_min[i] = nowstd->states[i];
				}
			}
			nowstd = nowstd->next;
		}
		tempstate_index = 0;
		tempsub = tempstate_max[0] - tempstate_min[0];
		for (i = 0; i < 10; i++)
		{
			if (tempstate_max[i] - tempstate_min[i] > tempsub)
			{
				tempsub = tempstate_max[i] - tempstate_min[i];
				tempstate_index = i;
			}
		}

		/*	Divide points	*/
		int leftstd_num = 0, rightstd_num = 0;
		nowstd = trainstd->next;
		//root = new kdNode;
		root->dividestate = tempstate_index;
		root->divideval = (float)((tempstate_max[tempstate_index] + tempstate_min[tempstate_index]) / 2.0);
		root->leftchild = new kdNode;
		root->rightchild = new kdNode;
		root->points = NULL;	// mid point
		DATALIST left_trainstd = new dataNode, right_trainstd = new dataNode;
		left_trainstd->states = NULL, right_trainstd->states = NULL;
		left_trainstd->next = NULL, right_trainstd->next = NULL;
		DATALIST nowstd_left = left_trainstd, nowstd_right = right_trainstd;

		while (nowstd)
		{
			if (nowstd->states[tempstate_index] <= root->divideval)
			{
				nowstd_left->next = nowstd;
				nowstd = nowstd->next;
				nowstd_left = nowstd_left->next;
				nowstd_left->next = NULL;
				leftstd_num++;
			}
			else
			{
				nowstd_right->next = nowstd;
				nowstd = nowstd->next;
				nowstd_right = nowstd_right->next;
				nowstd_right->next = NULL;
				rightstd_num++;
			}
		}

		free(nowstd);
		free(trainstd);
		BuidKDTree(left_trainstd, profile, leftstd_num, root->leftchild);
		BuidKDTree(right_trainstd, profile, rightstd_num, root->rightchild);
	}
	else
	{
		int i = 0;
		//root = new kdNode;
		root->dividestate = -1;
		root->divideval = -1.0;
		root->leftchild = NULL;
		root->rightchild = NULL;
		root->points = new dataNode;	// leaf point
		root->points->states = new float[10];	// 22-31
		for (i = 0; i < 10; i++)
		{
			root->points->states[i] = trainstd->next->states[i];
		}
		root->points->next = trainstd->next->next;
	}
}

int ComputeDeviation(KDTree root, char* testfile, float* maxstate, float* minstate, char* resultfile)
{
	KDTree tempkd;
	int resultnum = 0;
	int count = 0, statenum = 10, i = 0, j = 0;
	float tempnum = 0.0, tempdistance = 0.0, distance1 = 0.0, distance2 = 0.0;
	float newmax = 1.0, newmin = 0.0;
	float *states = new float[statenum];
	char *substr, outstr[100] = { 0 };
	char str[256] = { 0 }, tempstr[100] = { 0 };
	DATALIST tempdata = NULL;
	int flag = 0;

	ifstream testin(testfile);
	ofstream resultout(resultfile);
	while (!testin.eof())
	{
		testin.getline(str, sizeof(str));

		/* Get label */
		j = (int)(strlen(str) - 1);
		while (str[j] != ',')
		{
			j--;
		}
		memcpy(tempstr, str + j + 1, strlen(str) - 1 - j);

		/* Get states */
		count = 0;
		substr = strtok(str, ",");
		while (substr)
		{
			if (count >= 21 && count <= 30)
			{
				tempnum = (float)atof(substr);
				states[count - 21] = tempnum;
			}
			substr = strtok(NULL, ",");
			count++;
		}

		/* Std */
		for (i = 0; i < statenum; i++)
		{
			if (maxstate[i] != minstate[i])
			{
				states[i] = (states[i] - minstate[i]) * (newmax - newmin) / (maxstate[i] - minstate[i]) + newmin;
			}
			else
			{
				states[i] = newmin;
			}
		}

		/* Find profile */
		tempkd = root;
		while (!tempkd->points)
		{
			if (states[tempkd->dividestate] <= tempkd->divideval)
			{
				tempkd = tempkd->leftchild;
			}
			else
			{
				tempkd = tempkd->rightchild;
			}
		}

		/* Compute deviation */
		tempdata = tempkd->points;
		distance1 = 10.0;	// distance <= (10)^0.5
		distance2 = 10.0;
		while (tempdata)
		{
			tempdistance = 0.0;
			for (i = 0; i < statenum; i++)
			{
				tempdistance += powf((tempdata->states[i] - states[i]), 2);
			}
			tempdistance = sqrtf(tempdistance);
			if (tempdistance <= distance1)
			{
				distance1 = tempdistance;
				distance2 = distance1;
			}
			else if (tempdistance <= distance2)
			{
				distance2 = tempdistance;
			}
			tempdata = tempdata->next;
		}

		/* Write result.txt */
		resultout << "(";
		for (i = 0; i < statenum; i++)
		{
			sprintf(outstr, "%f", states[i]);
			resultout << outstr;
			if (i + 1 < statenum)
			{
				resultout << ",";
			}
		}
		resultout << ")";
		sprintf(outstr, "%f", (distance1 + distance2) / 2.0);	// (D1 + D2) / 2
		//sprintf(outstr, "%f", distance1);	// (D1 + D2) / 2
		resultout << " " << outstr << " " << tempstr;
		if (!testin.eof())
		{
			resultout << endl;
		}
		resultnum++;

		memset(str, 0, sizeof(str));
		memset(tempstr, 0, sizeof(tempstr));
		memset(outstr, 0, sizeof(outstr));
	}
	testin.close();
	resultout.close();

	return resultnum;
}

void SetThreshold_Statistic(char* resultfile, int resultnum)
{
	int i = 0, j = 0, count = 0;
	int ThresholdIndex = 0;
	int TPnum = 0, FPnum = 0, FNnum = 0, TNnum = 0;		// True Positive, False Positive, False Negative, True Negative
	int thr_TPnum = 0, thr_FPnum = 0, thr_FNnum = 0, thr_TNnum = 0;
	float TPR = 0.0, FPR = 0.0, tempsub = 0.0;							// true positive rate(TPR), false positive rate(FPR)
	char tempstr[20] = { 0 }, *substr, str[256] = { 0 };
	float tempdev = 0.0, *deviation = new float[resultnum];
	char **label = new char*[resultnum];
	for (i = 0; i < resultnum; i++)
	{
		deviation[i] = 0.0;
	}
	for (i = 0; i < resultnum; i++)
	{
		label[i] = new char[20];
		memset(label[i], 0, 20);
	}

	/*	Read result.txt	*/
	i = 0;
	ifstream resultin(resultfile);
	while (!resultin.eof() && i < resultnum)
	{
		resultin.getline(str, sizeof(str));

		count = 0;
		substr = strtok(str, " ");
		while (substr)
		{
			if (count == 1)
			{
				deviation[i] = (float)atof(substr);
			}
			if (count == 2)
			{
				strcpy(label[i], substr);
			}
			substr = strtok(NULL, " ");
			count++;
		}
		memset(str, 0, sizeof(str));
		i++;
	}
	resultin.close();

	/*	Sort by Threshold	*/
	for (i = 0; i < resultnum - 1; i++)
	{
		for (j = i + 1; j < resultnum; j++)
		{
			if (deviation[j] < deviation[i])
			{
				tempdev = deviation[i];
				deviation[i] = deviation[j];
				deviation[j] = tempdev;

				memset(tempstr, 0, sizeof(tempstr));
				strcpy(tempstr, label[i]);
				memset(label[i], 0, sizeof(label[i]));
				strcpy(label[i], label[j]);
				memset(label[j], 0, sizeof(label[j]));
				strcpy(label[j], tempstr);
			}
		}
	}

	ofstream resultout("resultsort.txt");
	for ( i = 0; i < resultnum; i++)
	{
		sprintf(tempstr, "%f", deviation[i]);
		resultout << tempstr << " " << label[i] << endl;
	}
	resultout.close();

	/*	find the best threshold	*/
	tempsub = 1.0;
	ThresholdIndex = 0;
	thr_FNnum = 0; thr_FPnum = 0;
	for (count = 0; count < resultnum; count++)
	{
		TPnum = 0, FPnum = 0, FNnum = 0, TNnum = 0;
		for (i = 0; i <= count; i++)	// check normal
		{
			if (strcmp(label[i], "normal.") == 0)	// real normal
			{
				TPnum++;
			}
			else
			{
				FPnum++;
			}
		}
		for (i = count + 1; i < resultnum; i++)	// check abnormal
		{
			if (strcmp(label[i], "normal.") == 0)	// real normal
			{
				FNnum++;
			}
			else
			{
				TNnum++;
			}
		}

		TPR = (float)(TPnum) / (float)(TPnum + FNnum);
		FPR = (float)(FPnum) / (float)(FPnum + TNnum);
		if (tempsub > fabsf(TPR + FPR - 1))
		{
			tempsub = fabsf(TPR + FPR - 1);
			ThresholdIndex = count;
			thr_FNnum = FNnum;
			thr_FPnum = FPnum;
			thr_TNnum = TNnum;
			thr_TPnum = TPnum;
		}
	}

	/* Statistic */
	cout << "选择的攻击数据：\t" << (int)(resultnum / 2.0) << endl;
	cout << "检测出攻击数据：\t" <<  thr_TNnum << endl;
	cout << "误报率：" << setprecision(4) << ((float)(thr_FNnum) / (float)(thr_FNnum + thr_TPnum)) * 100.0 << '%' << endl;
	//cout << "误报率：" << setprecision(4) << ((float)(thr_FNnum) / (float)(resultnum)) * 100.0 << '%' << endl;
	cout << "漏报率：" << setprecision(4) << ((float)(thr_FPnum) / (float)(thr_TNnum + thr_FPnum)) * 100.0 << '%' << endl;
}

int main(int argc, char *argv[])
{
	int i = 0, profile = 10, statenum = 10;		// Predefined
	float *maxstate = NULL, *minstate = NULL;
	char rawfile[] = "kddcup.data_10_percent";
	//char rawfile[] = "rawdata.txt";
	maxstate = new float[statenum];
	minstate = new float[statenum];
	for (i = 0; i < statenum; i++)
	{
		maxstate[i] = -1.0;
		minstate[i] = -1.0;
	}
	RawDataProcess(rawfile, maxstate, minstate);
	cout << "maxstate:" << endl;
	for (i = 0; i < 10; i++)
	{
		cout << maxstate[i] << "\t";
	}
	cout << endl;
	cout << "minstate:" << endl;
	for (i = 0; i < 10; i++)
	{
		cout << minstate[i] << "\t";
	}
	cout << endl;

	char trainfile[] = "train.txt";
	char trainstdfile[] = "trainstd.txt";
	DATALIST trainstd = TrainDataStd(trainfile, trainstdfile, maxstate, minstate);

	KDTree root = new kdNode;
	DATALIST tempstd = trainstd->next;
	int num = 0;
	while (tempstd)
	{
		num++;
		tempstd = tempstd->next;
	}
	BuidKDTree(trainstd, profile, num, root);

	char testfile[] = "test.txt";
	char resultfile[] = "result.txt";
	int resultnum = ComputeDeviation(root, testfile, maxstate, minstate, resultfile);

	SetThreshold_Statistic(resultfile, resultnum);

	//cout << "test" << endl;
	char ch;
	cin >> ch;
	return 0;
}