#include "widget.h"
#include "ui_widget.h"

#include <QtMath>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QMediaPlayer>

#include <QVBoxLayout>

#include <QDebug>


#define samplerate         44100 // частота сэмпла
#define wavefrequency      440   // частота волны
#define wavevolume         32767 // размах амплитуды

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    QChart *m_chart = new QChart;
    QLineSeries *m_series = new QLineSeries;

    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);

    chartView->setMinimumSize(800, 600);
    m_chart->addSeries(m_series);

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 100);
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-35000, 35000);
    axisY->setTitleText("Audio level");
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);
    m_chart->legend()->hide();
    m_chart->setTitle("Type of wave");


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
    mainLayout->addWidget(ui->pushButton);

    qreal period = samplerate/wavefrequency/2; //период волны

    for(int i=0;i<100;i++)//для вывода на график
    {
    int n = static_cast<int>(wavevolume * qSin(i*M_PI/period)); //sine
    m_series->append(i, n);
    }

    for(int i=0;i<16384;i++) //заполняем буфер
    {
    int n  = static_cast<int>(wavevolume * qSin(i*M_PI/period)); //вычисление sine

    buffer.append(char(n));
    buffer.append(char(n>>8));
    }

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QAudioFormat format;
    format.setSampleRate(samplerate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo deviceOut(QAudioDeviceInfo::defaultOutputDevice());
    qDebug()<<"Selected Output device ="<<deviceOut.deviceName();


    if (!deviceOut.isFormatSupported(format))
    format = deviceOut.nearestFormat(format);

    m_audioOutput = new QAudioOutput(format, this);


    buf = new QBuffer(&buffer);
    buf->open(QIODevice::ReadOnly);

    m_audioOutput->start(buf);


}
