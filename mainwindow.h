#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager_ptr; // pointer for managing network access
    QMessageBox *search_prompt; // pointer for 'searching' message box

private slots:
    void on_pushButtonSearch_clicked(); // slot for sending the request to the API after inserting movie title and pressing search button or enter on keyboard
    void onReplyFinished(QNetworkReply *reply_ptr); // slot for handling the content received from the request
};

#endif // MAINWINDOW_H
