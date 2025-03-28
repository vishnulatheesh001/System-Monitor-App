#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateUI);
    timer->start(1000);

}

void MainWindow::updateUI() {
    QFile file("C:/temp/sysmon_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString lastLine;

    // Read the last line (latest temperature data)
    while (!in.atEnd()) {
        lastLine = in.readLine();
    }

    file.close();

    if (!lastLine.isEmpty()) {
        QStringList data = lastLine.split("\t");
        if (data.size() == 2) {
            QString timeStr = data[0];
            QString tempStr = data[1];

            // Update QLabel (time)
            ui->timeLabel->setText("Time: " + timeStr);

            // Update QProgressBar (temperature)
            double temp = tempStr.toDouble();
            ui->tempProgressBar->setRange(0, 100); // Set range (adjust if needed)
            ui->tempProgressBar->setValue(static_cast<int>(temp));

            qDebug() << "Updated time:" << timeStr << ", Temperature:" << temp;
        }
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}


