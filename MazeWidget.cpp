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

#include "MazeWidget.h"

MazeWidget::MazeWidget(QWidget *parent)
    : QWidget(parent)
{
    //first=true;
    direct=0;
    M=11;
    N=11;
    this->setWindowTitle("迷宫游戏");
    this->resize((N+7)*20,(M+4)*20);
    this->setFocus(Qt::MouseFocusReason);

    findwaybtn=new QPushButton(this);
    findwaybtn->setText("寻找路径");
    findwaybtn->setGeometry((N+1)*20,30,60,20);
    findwaybtn->setEnabled(false);

    findshortwaybtn=new QPushButton(this);
    findshortwaybtn->setText("最短路径");
    findshortwaybtn->setGeometry((N+1)*20,60,60,20);
    findshortwaybtn->setEnabled(false);

    resetbtn=new QPushButton(this);
    resetbtn->setText("重新开始");
    resetbtn->setGeometry((N+1)*20,90,60,20);
    resetbtn->setEnabled(false);

    direction=new QComboBox(this);
    direction->addItem(QString("4方向"));
    direction->addItem(QString("8方向"));

    //创建迷宫矩阵
    matrix=new Maze*[M];
    for(int i=0;i<M;i++)
    {
        matrix[i]=new Maze[N];
    }
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            matrix[i][j].state=0;//初始化迷宫矩阵，将所有节点设置为墙
            matrix[i][j].i=i;
            matrix[i][j].j=j;
        }
    //随机生成种子数
    QTime time=QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    MazeStack=new stack<Maze>;
    FinalPath=new vector<Maze>;
    BoxStack=new stack<Box>;
    Box1Stack=new stack<Box1>;
    Box1Queue=new queue<Box1>;
    X=1;
    Y=1;

    connect(findwaybtn,SIGNAL(clicked()),this,SLOT(findwaybtnSlot()));

    connect(findshortwaybtn,SIGNAL(clicked()),this,SLOT(findshortwaybtnSlot()));
    connect(resetbtn,SIGNAL(clicked()),this,SLOT(resetSlot()));
    //连接下拉列表框
    connect(direction,SIGNAL(currentIndexChanged(int)),this,SLOT(changeDirect(int)));
    showMaze();
}

MazeWidget::~MazeWidget()
{

}

void MazeWidget::keyPressEvent(QKeyEvent *e)
{
    if(0)//chaiqiang)//如果拆墙
    {
        if(e->key()==87||e->key()==16777235)//上
        {
            if(X>1)
            {
                X=X-1;
            }
        }
        else if(e->key()==83||e->key()==16777237)//下
        {
            if(X<M-2)
            {
                X=X+1;
            }
        }
        else if(e->key()==65||e->key()==16777234)//左
        {
            if(Y>1)
            {
                Y=Y-1;
            }
        }
        else if(e->key()==68||e->key()==16777236)//右
        {

            if(Y<N-2)
            {
                Y=Y+1;
            }
        }
        matrix[X][Y].state=1;
        //当老鼠到达粮仓时，重新构建迷宫地图，使得存放路劲的容器置为空
        if(X==M-2&&Y==N-2)
        {
            QMessageBox::information(this,"提示","找到粮仓",QMessageBox::Yes);
            for(int i=0;i<M;i++)
                for(int j=0;j<N;j++)
                    matrix[i][j].state=0;
            this->CreatMaze();

            findwaybtn->setEnabled(true);
            findshortwaybtn->setEnabled(true);
            resetbtn->setEnabled(true);
            FinalPath->clear();
            X=M/2;
            Y=N/2;
            if(X%2==0)
                X+=1;
            if(Y%2==0)
                Y+=1;
        }
    }
    else
    {
        int k;
        if(e->key()==87||e->key()==16777235)//上
        {
            k=X-1;
            if(k>0&&matrix[k][Y].state!=0)
                X=k;
        }
        else if(e->key()==83||e->key()==16777237)//下
        {
            k=X+1;
            if(k<M-1&&matrix[k][Y].state!=0)
                X=k;
        }
        else if(e->key()==65||e->key()==16777234)//左
        {
            k=Y-1;
            if(k>0&&matrix[X][k].state!=0)
                Y=k;
        }
        else if(e->key()==68||e->key()==16777236)//右
        {
            k=Y+1;
            if(k<N-1&&matrix[X][k].state!=0)
                Y=k;
        }
        if(X==M-2&&Y==N-2)
        {
            QMessageBox::information(this,"提示","找到粮仓",QMessageBox::Yes);
            for(int i=0;i<M;i++)
                for(int j=0;j<N;j++)
                    matrix[i][j].state=0;
            this->CreatMaze();
            FinalPath->clear();
            X=M/2;
            Y=N/2;
            if(X%2==0)
                X+=1;
            if(Y%2==0)
                Y+=1;
        }

    }
    update();
}
void MazeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    for(int i=FinalPath->size()-1;i>=0;i--)
    {
        matrix[FinalPath->at(i).i][FinalPath->at(i).j].state=2;
        if(FinalPath->at(i).i==X&&FinalPath->at(i).j==Y)//当老鼠经过寻找迷宫路径时，使得state=1，即消除黄色方块
        {
            matrix[FinalPath->at(i).i][FinalPath->at(i).j].state=1;
            FinalPath->pop_back();
        }
    }

    if(1/*!first*/)
    {
        for(int i=0;i<M;i++)
            for(int j=0;j<N;j++)
            {
                if(i==X&&j==Y)//画老鼠
                {               
                    painter.setBrush(QBrush(Qt::green));
                    painter.drawEllipse(j*20,i*20,20,20);
                    painter.setPen(Qt::black);
                }
                else if(i==M-2&&j==N-2)//画粮仓
                {
                    painter.setBrush(QBrush(Qt::green));
                    painter.drawEllipse(j*20,i*20,20,20);
                    painter.setPen(Qt::black);
                }
                else
                {
                    switch(matrix[i][j].state)
                    {
                    case 0://画墙壁
                        painter.setBrush(QBrush(Qt::darkGray,Qt::SolidPattern));
                        painter.drawRect(QRect(j*20,i*20,20,20));
                        break;
                    case 1://画通路
                        painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
                        painter.drawRect(QRect(j*20,i*20,20,20));
                        break;
                    case 2://画迷宫寻找路径
                        painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
                        painter.drawRect(QRect(j*20,i*20,20,20));
                        break;
                    }
                }
            }
    }
}


void MazeWidget::showMaze()
{
    if(judgeisJiShu())
    {
        if(true)
        {
            //删除原先matrix数组
            for(int i=0;i<M;i++)
            delete [] matrix[i];
            delete [] matrix;
            M=6;
            N=8;
            this->resize((N+4)*20,(M)*20+1);
            findshortwaybtn->setGeometry((N+1)*20,50,60,20);
            resetbtn->setGeometry((N+1)*20,90,60,20);
            findwaybtn->setGeometry((N+1)*20,30,60,20);
            FinalPath->clear();
            findwaybtn->setEnabled(true);
            resetbtn->setEnabled(true);
            findshortwaybtn->setEnabled(true);
            direction->setGeometry((N+1)*20,10,60,20);
            this->setFocus(Qt::MouseFocusReason);
            X=1;
            Y=1;
            if(X%2==0)
                X+=1;
            if(Y%2==0)
                Y+=1;
            //重新创建新matrix数组，因为M和N发生了改变
            matrix=new Maze*[M];
            for(int i=0;i<M;i++)
            {
                matrix[i]=new Maze[N];
            }
            for(int i=0;i<M;i++)
                for(int j=0;j<N;j++)
                {
                    matrix[i][j].state=0;//初始化迷宫矩阵，将所有节点设置为墙
                    matrix[i][j].i=i;
                    matrix[i][j].j=j;
                }
            this->CreatMaze();
            //first=false;
            update();
        }
        else
        {
            QMessageBox::information(this,"警告","输入的数不能小于5",QMessageBox::Yes);

        }
    }
    else
    {
        QMessageBox::information(this,"警告","请输入奇数",QMessageBox::Yes);

    }
}
void MazeWidget::resetSlot()
{
    showMaze();
}

bool MazeWidget::judgeisJiShu()
{
    return true;
}
//创建迷宫节点矩阵
void MazeWidget::CreatMaze()
{

    matrix[1][1].state=1;
    matrix[1][2].state=1;
    matrix[1][4].state=1;
    matrix[1][5].state=1;
    matrix[1][6].state=1;

    matrix[2][2].state=1;
    matrix[2][3].state=1;
    matrix[2][4].state=1;
    matrix[2][5].state=1;
    matrix[2][6].state=1;

    matrix[3][1].state=1;
    matrix[3][2].state=1;
    matrix[3][4].state=1;
    matrix[3][6].state=1;
    matrix[4][1].state=1;
    matrix[4][2].state=1;
    matrix[4][3].state=1;
    matrix[4][4].state=1;
    matrix[4][5].state=1;
    matrix[4][6].state=1;





}
//用深度优先遍历的方法寻找路径,将路径数据保存在FinalPath容器中
void MazeWidget::Findway()
{
    FinalPath->clear();
    int i,j,di,find;
    Maze mm[M][N];
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            mm[i][j].state=matrix[i][j].state;
            mm[i][j].i=i;
            mm[i][j].j=j;
        }
    Box box;
    box.i=X;
    box.j=Y;
    box.di=-1;
    BoxStack->push(box);
    mm[X][Y].state=-1;
    while(1)
    {
        i=BoxStack->top().i;
        j=BoxStack->top().j;
        di=BoxStack->top().di;
        if(i==M-2&&j==N-2)//如果当前栈顶为粮仓节点，将路径存放劲FinalPath
        {
            while(!BoxStack->empty())
            {
                Maze temp;
                temp.i=BoxStack->top().i;
                temp.j=BoxStack->top().j;
                temp.state=1;
                FinalPath->push_back(temp);
                BoxStack->pop();
            }
            return;
        }
        find=0;
        while((find==0&&direct==0&&di<4)||(find==0&&direct==1&&di<8))
        {
            di++;
            //原来的4方向
            if(direct==0){
                switch(di)
                {
                case 0:i=BoxStack->top().i-1;j=BoxStack->top().j;break;//向上寻路
                case 1:i=BoxStack->top().i;j=BoxStack->top().j+1;break;//向右寻路
                case 2:i=BoxStack->top().i+1;j=BoxStack->top().j;break;//向下寻路
                case 3:i=BoxStack->top().i;j=BoxStack->top().j-1;break;//向左寻路
                }
            }
            //八方向
            else{
                switch(di)
                {

                case 1:i=BoxStack->top().i;j=BoxStack->top().j+1;break;//向右寻路
                case 2:i=BoxStack->top().i+1;j=BoxStack->top().j;break;//向下寻路
                case 7:i=BoxStack->top().i+1;j=BoxStack->top().j+1;break;//向右下寻路
                case 3:i=BoxStack->top().i;j=BoxStack->top().j-1;break;//向左寻路
                case 4:i=BoxStack->top().i-1;j=BoxStack->top().j-1;break;//向左上寻路
                case 5:i=BoxStack->top().i-1;j=BoxStack->top().j+1;break;//向右上寻路
                case 6:i=BoxStack->top().i+1;j=BoxStack->top().j-1;break;//向左下寻路
                case 0:i=BoxStack->top().i-1;j=BoxStack->top().j;break;//向上寻路
                }
            }
            if(mm[i][j].state==1)
            {
                find=1;
            }
        }
        if(find==1)
        {
            mm[i][j].state=-1;
            BoxStack->top().di=di;
            box.i=i;
            box.j=j;
            box.di=-1;
            BoxStack->push(box);
        }
        else
        {
            BoxStack->pop();
        }
    }
}

void MazeWidget::changeDirect(int m)
{
    direct=m;
}

void MazeWidget::findwaybtnSlot()
{
      //使得迷宫矩阵的state为2的值变为1
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(matrix[i][j].state==2)
            {
                matrix[i][j].state=1;
            }
        }
    this->Findway();
    this->setFocus(Qt::MouseFocusReason);
    update();
}

//用广度优先遍历的方法寻找迷宫路径，寻找的路径是最短路径
void MazeWidget::FindShortWay()
{
    FinalPath->clear();
    while(!Box1Queue->empty())
        Box1Queue->pop();
    int i,j,find=0,di,k=0,count=0;
    Maze mm[M][N];
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            mm[i][j].state=matrix[i][j].state;
            mm[i][j].i=i;
            mm[i][j].j=j;
        }
    Box1 box1;
    Maze temp;
    box1.i=X;
    box1.j=Y;
    box1.pre=-1;
    box1.num=k;
    Box1Queue->push(box1);
    mm[X][Y].state=-1;
    while(!Box1Queue->empty()&&!find)
    {
        i=Box1Queue->front().i;
        j=Box1Queue->front().j;
        box1.i=i;
        box1.j=j;
        box1.num=Box1Queue->front().num;
        box1.pre=Box1Queue->front().pre;
        Box1Stack->push(box1);
        if(i==M-2&&j==N-2)
        {
            find=1;
            while(!Box1Stack->empty())
            {
                temp.i=Box1Stack->top().i;
                temp.j=Box1Stack->top().j;
                temp.state=1;
                FinalPath->push_back(temp);
                count=Box1Stack->top().pre;//记录当前的路径节点的上一个坐标的下表
                while(!Box1Stack->empty()&&count!=Box1Stack->top().num)
                {
                    Box1Stack->pop();
                }
            }
        }
        if(direct==1){
        for(di=0;di<8;di++)
        {
            switch(di)//0上1右2下3左
            {
            case 0:i=Box1Queue->front().i-1;j=Box1Queue->front().j;break;
            case 1:i=Box1Queue->front().i;j=Box1Queue->front().j+1;break;
            case 2:i=Box1Queue->front().i+1;j=Box1Queue->front().j;break;
            case 3:i=Box1Queue->front().i;j=Box1Queue->front().j-1;break;
            case 4:i=Box1Queue->front().i-1;j=Box1Queue->front().j-1;break;
            case 5:i=Box1Queue->front().i-1;j=Box1Queue->front().j+1;break;
            case 6:i=Box1Queue->front().i+1;j=Box1Queue->front().j-1;break;
            case 7:i=Box1Queue->front().i+1;j=Box1Queue->front().j+1;break;
            }
            if(mm[i][j].state==1)
            {
                k++;
                box1.i=i;
                box1.j=j;
                box1.num=k;
                box1.pre=Box1Queue->front().num;
                Box1Queue->push(box1);
                mm[i][j].state=-1;
            }
        }
        }
        else if(direct==0){
            for(di=0;di<8;di++)
            {
                switch(di)//0上1右2下3左
                {
                case 0:i=Box1Queue->front().i-1;j=Box1Queue->front().j;break;
                case 1:i=Box1Queue->front().i;j=Box1Queue->front().j+1;break;
                case 2:i=Box1Queue->front().i+1;j=Box1Queue->front().j;break;
                case 3:i=Box1Queue->front().i;j=Box1Queue->front().j-1;break;

                }
                if(mm[i][j].state==1)
                {
                    k++;
                    box1.i=i;
                    box1.j=j;
                    box1.num=k;
                    box1.pre=Box1Queue->front().num;
                    Box1Queue->push(box1);
                    mm[i][j].state=-1;
                }
            }
        }
        Box1Queue->pop();
    }
}
void MazeWidget::findshortwaybtnSlot()
{
    //使得迷宫矩阵的state为2的值变为1
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(matrix[i][j].state==2)
            {
                matrix[i][j].state=1;
            }
        }
    this->FindShortWay();
    this->setFocus(Qt::MouseFocusReason);
    update();
}
