#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager_ptr(new QNetworkAccessManager(this)) // declare the pointer of network access manager
{
    ui->setupUi(this);
    setWindowTitle("Movie Info"); // set the title for the main window

    // for clearing the 'texts' of the labels present when app opens initially
    ui->labelTitle->clear();
    ui->labelGenre->clear();
    ui->labelSummary->clear();
    ui->labelDirector->clear();
    ui->labelRating->clear();
    ui->labelPoster->clear();
    ui->labelActor->clear();
    ui->labelRated->clear();
    ui->labelRuntime->clear();
    ui->labelType->clear();

    // connecting the signal and the slots
    // initially, the app only starts to 'search' if the search button is clicked
    // but with this, after the movie title is inserted, the 'enter' button on the keyboard will do the same thing
    // without clicking the search button
    connect(ui->inputMovieTitle, &QLineEdit::returnPressed, this, &MainWindow::on_pushButtonSearch_clicked);
    // for receiving and handling the response that came after the request to the API
    connect(networkManager_ptr, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSearch_clicked() {
    // sets up the 'searching' message box
    search_prompt = new QMessageBox(this);
    search_prompt->setWindowTitle("Searching");
    search_prompt->setText("Please Wait!");
    search_prompt->setStandardButtons(QMessageBox::Close);
    search_prompt->setIcon(QMessageBox::Information);
    search_prompt->show();

    // sets up the placeholder text when the search bar is empty and not being actively clicked by the pointer of the mouse
    ui->inputMovieTitle->setPlaceholderText("Enter Movie Title Here...");
    // declares movie_title and sets the value to whatever text the user type in the line edit (search bar)
    QString movie_title = ui->inputMovieTitle->text();

    // conditioning based on the value of movie_title
    if(movie_title.isEmpty()) {
        // if movie_title doesn't contain any text, then gives this warning
        QMessageBox::warning(this, "Input Error!", "Please Input Movie Title!"); return;
    } else {
        // if movie_title actually has text, then initiate the request to the API
        QString api_key = "your-api-key"; // this is where you should put your api key
        QString req_url = QString("http://www.omdbapi.com/?t=%1&apikey=%2").arg(movie_title, api_key); // here, we set up the link for the request and insert the movie_title and the api_key as paramater in the link
        // pay attention to the placement of %1 and %2 (t for movie title, apikey obviously for the API key)
        // since this app only search for movies/series/shows/etc based on title
        // API keys are unique from one user to another

        // This line actually sends the request to the web service and gets the content of the request
        networkManager_ptr->get(QNetworkRequest(QUrl(req_url)));
    }
}

void MainWindow::onReplyFinished(QNetworkReply *reply_ptr) {
    // this line is for closing the earlier 'searching' message box, since if the program enters this slot, safe to assume that the request was finished and the program got the response (either successful or failed)
    search_prompt->close();
    // some movies/series/shows/etc doesn't have a poster, so if you kept searching, the next search result could show the poster of the previous search result
    // this line is to prevent that by clearing the content of the 'label' poster
    ui->labelPoster->clear();

    // conditioning based on the reply from thw web service
    if(reply_ptr->error()==QNetworkReply::NoError) {
        // if the response doesn't return any error, then program goes here
        // from the reply pointer, the program reads all the content received and store it in 'response'
        QByteArray response = reply_ptr->readAll();
        // parse Json file and convert it to QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        // retrieve the objects from the QJsonDocument
        QJsonObject jsonObj = jsonDoc.object();

        // conditioning based on the response
        if(jsonObj.contains("Response") && jsonObj["Response"].toString()=="True") {
            // if the jsonObj actually contains a response and is filled, then the program goes here

            // here, we try to store the value of each information in their own variables
            // the variable will contain 'N/A' if the value of the key specified down here is not found
            QString movie_title = jsonObj.contains("Title") ? jsonObj["Title"].toString() : "N/A";
            QString movie_year = jsonObj.contains("Year") ? jsonObj["Year"].toString() : "N/A";
            QString movie_genre = jsonObj.contains("Genre") ? jsonObj["Genre"].toString() : "N/A";
            QString movie_summary = jsonObj.contains("Plot") ? jsonObj["Plot"].toString() : "N/A";
            QString movie_director = jsonObj.contains("Director") ? jsonObj["Director"].toString() : "N/A";
            QString movie_rating = jsonObj.contains("imdbRating") ? jsonObj["imdbRating"].toString() : "N/A";
            QString movie_poster = jsonObj.contains("Poster") ? jsonObj["Poster"].toString() : "N/A";
            QString movie_actor = jsonObj.contains("Actors") ? jsonObj["Actors"].toString() : "N/A";
            QString movie_rated = jsonObj.contains("Rated") ? jsonObj["Rated"].toString() : "N/A";
            QString movie_runtime = jsonObj.contains("Runtime") ? jsonObj["Runtime"].toString() : "N/A";
            QString media_type = jsonObj.contains("Type") ? jsonObj["Type"].toString() : "N/A";

            // here, we set the value of each information for display in the UI
            ui->labelTitle->setText(movie_title+" "+"("+movie_year+")");
            ui->labelTitle->setWordWrap(true); // sets up word wrap so the content doesn't get cut in the "box" of the label
            ui->labelTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding); // [(horizontal, vertical)]
            // sets up so that if word wrap isn't enough, the "box" for the label can be extended downward vertically
            // horizontal preferred (not changing from the initial size), but vertical can be expanding (can be changed from the initial size)

            ui->labelGenre->setText("Genre: "+movie_genre);
            ui->labelGenre->setWordWrap(true);
            ui->labelGenre->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelSummary->setText("Summary: "+movie_summary);
            ui->labelSummary->setWordWrap(true);
            ui->labelSummary->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelDirector->setText("Director: "+movie_director);
            ui->labelDirector->setWordWrap(true);
            ui->labelDirector->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelRating->setText("IMDb Rating: "+movie_rating);
            ui->labelRating->setWordWrap(true);
            ui->labelRating->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelActor->setText("Actors: "+movie_actor);
            ui->labelActor->setWordWrap(true);
            ui->labelActor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelRated->setText("Rated: "+movie_rated);
            ui->labelRated->setWordWrap(true);
            ui->labelRated->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelRuntime->setText("Duration: "+movie_runtime);
            ui->labelRuntime->setWordWrap(true);
            ui->labelRuntime->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->labelType->setText("Type: "+media_type);
            ui->labelType->setWordWrap(true);
            ui->labelType->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

            // here, we specifically sets up the image manager and how it will be displayed
            // first, create the QNetworkAccessManager instance
            QNetworkAccessManager *imgManager_ptr = new QNetworkAccessManager(this);
            // second, connect the signal to this specified/lambda slot
            connect(imgManager_ptr, &QNetworkAccessManager::finished, this, [this](QNetworkReply *imgReply_ptr) {
                // conditioning based on if the image can be retrieved from the website the json object gave earlier (since the json document earlier gave the link to the image rather than the image itself
                if(imgReply_ptr->error()==QNetworkReply::NoError) {
                    // if there's no error, the program goes here
                    // sets up QPixmap for the poster
                    QPixmap poster;
                    // read the image/picture from the link
                    poster.loadFromData(imgReply_ptr->readAll());
                    // set the image (QPixMap) dimension/size and how will it respond to changing main window's size
                    // it will also scaled based on the "box" where the poster will be placed in the main window (ui->labelPoster->size())
                    // it will also keeps the original image's aspect ratio
                    ui->labelPoster->setPixmap(poster.scaled(ui->labelPoster->size(), Qt::KeepAspectRatio));
                }
                // delete the network reply later after the image has been processed or there's an error in the reply
                imgReply_ptr->deleteLater();
            });
            // request the image from the link given in the json earlier (no API key needed here)
            imgManager_ptr->get(QNetworkRequest(QUrl(movie_poster)));
        } else {
            // if the request from earlier is success, but the web service can't find anything based on the user's text (movie_title) or the response is empty, then the program goes here and gives out a warning
            QMessageBox::warning(this, "Movie Not Found!", "There seems to be no match!");
        }
    } else {
        // if the response from earlier does return an error, then this warning is given out
        QMessageBox::warning(this, "Network Error", "Oh NOOOOOOOOOOOOOOOOOOO! It seems that there's a failure when fetching data from the API!");
    }

    // delete the network reply later after the information of the movie has been processed or there's an error in the reply
    reply_ptr->deleteLater();

    // force close the 'searching' message box if it hasn't closed earlier
    if(search_prompt) {
        search_prompt->close();
    }
}
