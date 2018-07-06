#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_inputPushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open Input Image", QDir::currentPath(), "Images (*.jpg *.png *.bmp)");
    if(QFile::exists(file_name))
    {
        m_ui->inputLineEdit->setText(file_name);
    }
}

void MainWindow::on_outputPushButton_clicked()
{
    QString file_name = QFileDialog::getSaveFileName(this, "Select Output Image", QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
    if(!file_name.isEmpty())
    {
        m_ui->outputLineEdit->setText(file_name);
        using namespace cv;
        Mat input_image, output_image;
        input_image = imread(m_ui->inputLineEdit->text().toStdString());
        if(m_ui->medianBlurRadioButton->isChecked())
            cv::medianBlur(input_image, output_image, 5);
        else if(m_ui->gaussianBlurRadioButton->isChecked())
            cv::GaussianBlur(input_image, output_image, Size(5, 5), 1.25);
        imwrite(file_name.toStdString(), output_image);
        if(m_ui->displayImageCheckBox->isChecked())
            imshow("Output Image", output_image);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::warning(this, "Exit", "Are you sure you want to close this program?", QMessageBox::Yes, QMessageBox::No);
    if(result == QMessageBox::Yes)
    {
        saveSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("Packt", "Hello_OpenCV_Qt", this);
    m_ui->inputLineEdit->setText(settings.value("inputLineEdit", "").toString());
    m_ui->outputLineEdit->setText(settings.value("outputLineEdit", "").toString());
    m_ui->medianBlurRadioButton->setChecked(settings.value("medianBlurRadioButton", true).toBool());
    m_ui->gaussianBlurRadioButton->setChecked(settings.value("gaussianBlurRadioButton", false).toBool());
    m_ui->displayImageCheckBox->setChecked(settings.value("displayImageCheckBox", false).toBool());
}

void MainWindow::saveSettings()
{
    QSettings settings("Packt", "Hello_OpenCV_Qt", this);
    settings.setValue("inputLineEdit", m_ui->inputLineEdit->text());
    settings.setValue("outputLineEdit", m_ui->outputLineEdit->text());
    settings.setValue("medianBlurRadioButton", m_ui->medianBlurRadioButton->isChecked());
    settings.setValue("gaussianBlurRadioButton", m_ui->gaussianBlurRadioButton->isChecked());
    settings.setValue("displayImageCheckBox", m_ui->displayImageCheckBox->isChecked());
}
