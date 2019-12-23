/*
    Gbaldraw2 - Multiplayer painting program
    Copyright (C) 2019  Jonathan Silverman

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "mainwindow.h"
#include <scribblearea.h>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>
#include "ui_mainwindow.h"
#include "server.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);
    createActions();
    createMenus();
    setWindowTitle("Gbaldraw");
    resize(1280, 690);
    //ui->setupUi(this);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(maybeSave()){
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::open(){
    if(maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open File"), QDir::currentPath());
        if(!fileName.isEmpty()){
            scribbleArea->openImage(fileName);
        }
    }
}

void MainWindow::save(){
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::penColor(){
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if(newColor.isValid()){
        scribbleArea->setPenColor(newColor);
    }
}

void MainWindow::penWidth(){
    bool ok;
    int newWidth = QInputDialog::getInt(this, "Gbaldraw2", tr("Select pen width : "), scribbleArea->penWidth(), 1, 50, 1, &ok);

    if(ok){
        scribbleArea->setPenWidth(newWidth);
    }
}

void MainWindow::about(){
    QMessageBox::about(this, tr("About Gbaldraw2"), "https://gbalda.com");
}

void MainWindow::startServer(){
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter username"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);

    if(ok) {
        scribbleArea->username = text;

            //server.serverPassword = text2;

            bool ok2;
            QString text2 = QInputDialog::getText(this, tr("Set server port"),
                                                 tr("Server port:"), QLineEdit::Normal,
                                                 "9999", &ok2);

            if(ok2){
                //server.serverPort = text3;
                server.scribbleArea = scribbleArea;
                server.serverPort = text2.toInt();
                server.startServer();
            }

    }
}

void MainWindow::startClient(){
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter ip:port"),
                                         tr("ip:port"), QLineEdit::Normal,
                                         "127.0.0.1:9999", &ok);

    if(ok) {

            QHostAddress address;
            address.setAddress(text.split(":").first());
            client.scribbleArea = scribbleArea;
            client.connectToServer(address, text.split(":").last().toInt());
    }
}

MainWindow::~MainWindow()
{
    //delete ui;
    delete scribbleArea;
    delete openAct;
    delete printAct;
    delete exitAct;
    delete penColorAct;
    delete penWidthAct;
//    delete clearScreenAct;
    delete aboutAct;
    delete aboutQtAct;
    delete startServerAct;
    delete startClientAct;
    delete saveAsMenu;
    delete fileMenu;
    delete optionMenu;
    delete networkMenu;
    delete helpMenu;

}

void MainWindow::createActions(){
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }
    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction(tr("&Pen Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

//    clearScreenAct = new QAction(tr("&Clear Screen..."), this);
//    clearScreenAct->setShortcut(tr("Ctrl+L"));
//    connect(clearScreenAct, SIGNAL(triggered()), scribbleArea, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About..."), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt..."), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    startServerAct = new QAction(tr("&Start Server"), this);
    connect(startServerAct, SIGNAL(triggered()), this, SLOT(startServer()));

    startClientAct = new QAction(tr("&Connect to Server"), this);
    connect(startClientAct, SIGNAL(triggered()), this, SLOT(startClient()));
}

void MainWindow::createMenus(){
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach(QAction *action, saveAsActs)
        saveAsMenu->addAction(action);
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    fileMenu->addAction(openAct);

    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
//    optionMenu->addAction(clearScreenAct);

    networkMenu = new QMenu(tr("&Network"), this);
    networkMenu->addAction(startServerAct);
    networkMenu->addAction(startClientAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(networkMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave(){
    if(scribbleArea->isModified()){
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Gbaldraw2"), tr("Modified. Wanna save?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if(ret == QMessageBox::Save) {
            return saveFile("png");
        } else if(ret==QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat) {
    QString initialPath = QDir::currentPath() + "untitled." + fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), initialPath, tr("%1 Files (*.%2;; All Files(*").arg(QString::fromLatin1(fileFormat.toUpper())).arg(QString::fromLatin1(fileFormat)));

    if(fileName.isEmpty()){
        return false;
    } else {
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}
