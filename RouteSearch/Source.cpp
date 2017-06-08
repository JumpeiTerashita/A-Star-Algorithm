#define SIZE_X 30	
#define SIZE_Y 30
#define START_Y 0
#define START_X 0
#define GOAL_Y 20	
#define GOAL_X 20

#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include "MT.h"

HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition = { 0,0 };
enum
{
	PANEL_FLOOR,
	PANEL_START,
	PANEL_GOAL,
	PANEL_WALL,
	PANEL_OPEN,
	PANEL_CLOSE,
	PANEL_SWAMP,
	PANEL_NICEROOT,
	PANEL_NICEROOTVEC
};



class Node
{
public:
	Node() {
		st = 0;
		parent = nullptr;
		RealCost = 0;
		EstimateCost = 0;
		Score = 0;
		tate = 0;
		yoko = 0;
	};
	~Node() {};

	int st;
	Node *parent;
	int RealCost;
	int EstimateCost;
	int Score;
	int tate;
	int yoko;
};

std::vector< Node* > NiceRoot;
std::list< Node* > OpenNaYatsura;
Node* panel[SIZE_X][SIZE_Y];
bool IsFinished = false;
bool IsNoRoot = false;
int FinalX = 0,FinalY = 0;

void CursorPositionSet(SHORT x, SHORT y) {
	CONSOLE_CURSOR_INFO cusor;
	GetConsoleCursorInfo(hConsoleOutput,&cusor);
	cusor.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &cusor);
	CursorPosition = {
		x,  // SHORT X
		y }; // SHORT Y
	SetConsoleCursorPosition(hConsoleOutput, CursorPosition);
}

static bool sortScore_Ascending(const Node* objA, const Node* objB)
{
	return objA->Score < objB->Score; //小さい順ソート
}

void PanelInit()
{
	for (int i = 0; i < SIZE_Y; i++)
	{
		for (int j = 0; j < SIZE_X; j++)
		{
			panel[i][j] = new Node();
			panel[i][j]->tate = i;
			panel[i][j]->yoko = j;
		}
	}

	for (int i = 0; i < SIZE_Y; i++)
	{
		for (int j = 0; j < SIZE_X; j++)
		{
			if (genrand_int31()%6 == 1)
			{
				panel[i][j]->st = PANEL_WALL;
			}
			else if (genrand_int31()%3==1)
			{
				panel[i][j]->st = PANEL_SWAMP;
			}
			
		}
	}
	

	panel[START_Y][START_X]->st = PANEL_START;
	panel[GOAL_Y][GOAL_X]->st = PANEL_GOAL;
}

void Disp()
{
	for (int i = 0; i < SIZE_Y; i++)
	{
		for (int j = 0; j < SIZE_X; j++)
		{
			switch (panel[j][i]->st)
			{
			case PANEL_WALL:
				printf("■");
				break;
			case PANEL_FLOOR:
				printf("□");
				break;
			case PANEL_START:
				printf("Ｓ");
				break;
			case PANEL_GOAL:
				printf("Ｇ");
				break;
			case PANEL_OPEN:
				printf("回");
				break;
			case PANEL_CLOSE:
				printf("×");
				break;
			case PANEL_SWAMP:
				printf("～");
				break;
			case PANEL_NICEROOT:
				printf("☆");
				break;
			case PANEL_NICEROOTVEC:
				printf("★");
				break;
			default:
				break;
			}

		}
		printf("\n");
	}
}



bool CanOpen(int _tate, int _yoko)
{
	if ((_tate >= 0 && _tate < SIZE_Y) && (_yoko >= 0 && _yoko < SIZE_X))
	{
		if (panel[_tate][_yoko]->st == PANEL_CLOSE ||panel[_tate][_yoko]->st == PANEL_OPEN||panel[_tate][_yoko]->st == PANEL_WALL)
		{
			return false;
		}
		return true;
	}
	return false;
	
}

void MoveBack(int _tate, int _yoko)
{
	NiceRoot.push_back(panel[_tate][_yoko]);
	panel[_tate][_yoko]->st = PANEL_NICEROOT;
	if (panel[_tate][_yoko]->parent == nullptr)
	{
		return;
	}
	int NextTate = panel[_tate][_yoko]->parent->tate;
	int NextYoko = panel[_tate][_yoko]->parent->yoko;
	
	CursorPositionSet(0,0);
	Disp();
	
	MoveBack(NextTate, NextYoko);
}

void ScoreCalc(int _tate, int _yoko)
{
	Node *NowPlate = panel[_tate][_yoko];

	if (panel[_tate][_yoko]->st == PANEL_GOAL)
	{
		IsFinished = true;
		FinalX = _tate;
		FinalY = _yoko;
		return;
	}


	//	実コスト計算
	if (NowPlate->parent != nullptr)
	{
		NowPlate->RealCost = NowPlate->parent->RealCost;
		if (NowPlate->st==PANEL_SWAMP) NowPlate->RealCost+=4;
		NowPlate->RealCost++;
	}

	//	推定コスト計算
	int Estima_X = abs(GOAL_Y - _yoko);
	int Estima_Y = abs(GOAL_X - _tate);
	NowPlate->EstimateCost = Estima_X + Estima_Y;

	//	スコア計算
	NowPlate->Score = NowPlate->RealCost + NowPlate->EstimateCost;

	NowPlate->st = PANEL_OPEN;
}


void OpenPanel(int _tate, int _yoko)
{

	if (CanOpen(_tate + 1, _yoko))
	{
		panel[_tate + 1][_yoko]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate + 1, _yoko);
		OpenNaYatsura.push_back(panel[_tate + 1][_yoko]);
	}

	if (CanOpen(_tate, _yoko + 1))
	{
		panel[_tate][_yoko + 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate, _yoko + 1);
		OpenNaYatsura.push_back(panel[_tate][_yoko + 1]);
	}
	if (CanOpen(_tate - 1, _yoko))
	{
		panel[_tate - 1][_yoko]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate - 1, _yoko);
		OpenNaYatsura.push_back(panel[_tate - 1][_yoko]);
	}
	if (CanOpen(_tate, _yoko - 1))
	{
		panel[_tate][_yoko - 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate, _yoko - 1);
		OpenNaYatsura.push_back(panel[_tate][_yoko - 1]);
	}
	/*if (CanOpen(_tate + 1, _yoko + 1))
	{
		panel[_tate + 1][_yoko + 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate + 1, _yoko + 1);
		OpenNaYatsura.push_back(panel[_tate + 1][_yoko + 1]);
	}
	if (CanOpen(_tate + 1, _yoko - 1))
	{
		panel[_tate + 1][_yoko - 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate + 1, _yoko - 1);
		OpenNaYatsura.push_back(panel[_tate + 1][_yoko - 1]);
	}
	if (CanOpen(_tate - 1, _yoko - 1))
	{
		panel[_tate - 1][_yoko - 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate - 1, _yoko - 1);
		OpenNaYatsura.push_back(panel[_tate - 1][_yoko - 1]);
	}
	if (CanOpen(_tate - 1, _yoko + 1))
	{
		panel[_tate - 1][_yoko + 1]->parent = panel[_tate][_yoko];
		ScoreCalc(_tate - 1, _yoko + 1);
		OpenNaYatsura.push_back(panel[_tate - 1][_yoko + 1]);
	}*/
	panel[_tate][_yoko]->st = PANEL_CLOSE;
}


int main()
{
	init_genrand((unsigned)time(NULL));
	PanelInit();

	Disp();
	getchar();

	OpenPanel(START_Y,START_X);
	CursorPositionSet(0, 0);
	Disp();
	


	while (!IsFinished&&!IsNoRoot)
	{
		OpenNaYatsura.sort(sortScore_Ascending);
		std::list< Node* >::iterator it = OpenNaYatsura.begin();
		OpenPanel((*it)->tate, (*it)->yoko);
		OpenNaYatsura.pop_front();
		CursorPositionSet(0, 0);
		Disp();
		

		if (OpenNaYatsura.size()==0)
		{
			IsNoRoot = true;
		}
	}

	if (IsNoRoot)
	{
		CursorPositionSet(0, 0);
		Disp();
		printf("\nチェックメイト！！\n");
		getchar();
		return 1;
	}
	MoveBack(FinalX, FinalY);
	
	CursorPositionSet(0, 0);
	Disp();
	

	for (int i = 0; i < NiceRoot.size(); i++)
	{
		NiceRoot[i]->st = PANEL_NICEROOTVEC;
	}

	CursorPositionSet(0, 0);
	Disp();
	getchar();
	return 0;
}