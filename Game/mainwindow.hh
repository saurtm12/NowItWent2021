﻿#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH
#include <dialog.hh>

#include "../Course/CourseLib/graphics/simpleactoritem.hh"
#include "../Course/CourseLib/core/location.hh"
#include "character.hh"
#include "city.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <iostream>
#include <memory>
#include <QVector>
#include <map>
#include <QString>

using namespace CourseSide;
using namespace Interface;
using namespace Model;
namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void setSize(int w, int h);
  void setTick(int t);

//  void addCharacter(Model::Character& character);
  void addActor(int locX, int locY, int type=0);
  void updateCoords(int nX, int nY);
  void readFiles();
  void startGame();
signals:
  void gameStarted();
  void keyPressed(int key);
private slots:
  void onStartButtonClicked();

private:
  void keyPressEvent(QKeyEvent *event) override;

  Ui::MainWindow *ui;
  City *city;
  QTimer *timer;
  QVector<QGraphicsItem*> actors_;
  SimpleActorItem* last_;

  //QVector<QGraphicsItem*> characters_;




  int width_ = 1095;
  int height_ = 592;
  int tick_ = 500;

};

#endif // MAINWINDOW_HH
