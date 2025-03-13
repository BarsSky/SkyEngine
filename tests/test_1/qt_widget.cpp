//
// Created by ubuntu on 28.05.24.
//

#include "qt_widget.h"

MainWindow::MainWindow(QT_Interface *ptr, QWidget *parent) : QMainWindow(parent)
{
    mgk_ptr = ptr;
    // central_widget = new QWidget(this);
    entropyWidget = new QWidget(this);
    setCentralWidget(entropyWidget);
}

MainWindow::~MainWindow()
{

    // delete layout;
}

void MainWindow::set_vulkan_widget(VkWidget *vulkan_widget)
{
    layout = new QGridLayout();
    magick_widget = vulkan_widget;
    layout->addWidget(magick_widget, 0, 0);
    entropyWidget->setLayout(layout);

    // entropyWidget->ShowWarning("AAAAAAAAAAAAффффффффффффффффффффффффффффффффффффффффф");
    magick_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    magick_widget->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    magick_widget->setFocus();
    update();
    magick_connect();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    magick_widget->closeEvent(event);
    QWidget::closeEvent(event);
}

void MainWindow::magick_connect()
{
    // выбор цели наблюдения
}

#include "moc_qt_widget.cpp"