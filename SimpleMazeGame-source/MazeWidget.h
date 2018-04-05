/**
 *Copyright:
 *This code is free software; you can redistribute it and/or modify
 *it under the terms of the GPL-3.0
 *
 * @author zhangjian
 * @date 2018.04
 *
 */

///////////////////////////////////////////////////////////////////////

#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H
#include <QWidget>
#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QPainter>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QPixmap>
using namespace std;
class Maze//定义迷宫节点类，节点类包括坐标以及一个整形数据用于指示是墙还是可以通过，0代表墙，1代表可以通过
{
public:
    int i;
    int j;
    int state;//0墙1通路
};

class Box//定义一个用于迷宫寻路存放迷宫节点信息的类
{
public:
    int i;
    int j;
    int di;//代表寻路四个方向 0上 1右 2下 3左,八方向的时候4左上，5右上，6左下，7右下
};
class Box1//定义一个用于迷宫寻最短路径存放迷宫节点的类
{
public:
    int i;
    int j;
    int pre;
    int num;
};
class MazeWidget : public QWidget
{
    Q_OBJECT

public:
    MazeWidget(QWidget *parent = 0);
    ~MazeWidget();
    void paintEvent(QPaintEvent *);
    bool judgeisJiShu();//判断lineedit输入是否为奇数
    void CreatMaze();//创建迷宫
    void keyPressEvent(QKeyEvent *);//重写时间
    void Findway();//深度优先遍历寻找迷宫路径
    void FindShortWay();//广度优先遍历寻找迷宫路径
    void showMaze();
private:
    Maze **matrix;//定义一个迷宫节点数组
    stack<Maze> *MazeStack;//定义一个栈用于存放迷宫节点
    vector<Maze> *FinalPath;//定义一个向量用于存放迷宫路径
    stack<Box> *BoxStack;//定义一个栈用于存放寻路节点
    queue<Box1> *Box1Queue;//定义一个队列用于存放寻最短路节点
    stack<Box1> *Box1Stack;//定义一个栈用于存放寻最短路节点
    int M;//迷宫矩阵的行
    int N;//迷宫矩阵的列
    int direct;

    QPushButton *resetbtn;
    QPushButton *findwaybtn;
    QPushButton *findshortwaybtn;
    QComboBox *direction;
    //老鼠的坐标
    int X;
    int Y;
private slots:
    void findwaybtnSlot();
    void findshortwaybtnSlot();
    void resetSlot();
    void changeDirect(int m);
};

#endif // MAZEWIDGET_H
