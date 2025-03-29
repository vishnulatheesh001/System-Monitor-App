#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("System Monitor App");

// Initialize QTableWidget with 11 rows and 2 columns
    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"Sysinfo", "Value"});
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnWidth(0, 130);

 // QTablewidget sysinfo section ................................................
    QStringList Sysinfo = {
        "Date & Time", "Temperature (°C)", "CPU Usage (%)",
        "Process Count", "Thread Count", "Handle Count",
        "Total RAM (MB)", "Used RAM (MB)",
        "Total Disk (GB)", "Free Disk (GB)"
    };

    for (int i = 0; i < Sysinfo.size(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(Sysinfo[i]));
    }


    // delay..............................................................
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateUI);
    timer->start(200);

    //initail updates
    updateUI();

}

void MainWindow::updateUI() {

    // File operations..........................................................
    QFile file("C:/temp/sysmon_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString lastLine;
    lastLine = in.readLine();
    file.close();

    if (!lastLine.isEmpty()) {
        QStringList data = lastLine.split("\t");
        if (data.size() == 10) {
            // Extract values
            QString timeStr = data[0];
            QString tempStr = data[1];
            int cpuUsage = data[2].toInt();
            int processCount = data[3].toInt();
            int threadCount = data[4].toInt();
            int handleCount = data[5].toInt();
            int totalMemory = data[6].toInt();
            int usedMemory = data[7].toInt();
            int totalDisk = data[8].toInt();
            int freeDisk = data[9].toInt();


            // Update QTableWidget Values section..................................................
            QStringList values = {
                timeStr, tempStr, QString::number(cpuUsage),
                QString::number(processCount), QString::number(threadCount), QString::number(handleCount),
                QString::number(totalMemory), QString::number(usedMemory),
                QString::number(totalDisk), QString::number(freeDisk)
            };

            for (int i = 0; i < values.size(); ++i) {
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem(values[i]));
            }

            ui->tableWidget->item(2, 1)->setForeground(QColor(Qt::green));
            ui->tableWidget->item(3, 1)->setForeground(QColor(Qt::green));
            ui->tableWidget->item(4, 1)->setForeground(QColor(Qt::green));
            ui->tableWidget->item(5, 1)->setForeground(QColor(Qt::green));
            ui->tableWidget->item(7, 1)->setForeground(QColor(Qt::green));

            // Qlable widget.............................................................
            ui->Process_label->setText(values[3]);
            ui->Thread_label->setText(values[4]);
            ui->Handles_label->setText(values[5]);
            ui->RAM_label->setText(values[7]);


            // progress bars ..............................................................
            ui->cpuProgressBar->setRange(0, 100);
            ui->cpuProgressBar->setValue(data[2].toInt());
            ui->memoryProgressBar->setRange(0, 100);
            ui->memoryProgressBar->setValue((static_cast<float>(usedMemory)/totalMemory) *100);
            ui->diskProgressBar->setRange(0, totalDisk);
            ui->diskProgressBar->setValue(freeDisk);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


