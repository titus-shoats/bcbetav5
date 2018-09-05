#pragma once
#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QVector>
#include <QByteArray>


class Options {

public:
        // valid options "google","bing", "yahoo"
        QString searchEngineOptions[3];
        //{"gmail","hotmail","yahoo","outlook"};
        QString emailOptions[4];
        //valid options {"instagram","facebook","soundcloud",etc};
        QString socialNetworkOptions[14];
        //valid options , keywords that user inserts;
        QString keywordSearchBoxOptions[1];
        //valid options , userAgent will hold the current user agent string
        QString userAgentsOptions[1];
        //valid options , load list of keywords into array;
        // QHash<QByteArray, int> keywordLoadListOptions;
        //valid options , load completed list/tagged of keywords into array;




};

#endif // OPTIONS_H
