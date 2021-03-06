#include "draftitemcounter.h"
#include "../themehandler.h"
#include <QtWidgets>

DraftItemCounter::DraftItemCounter(QObject *parent, QHBoxLayout *hLayout, QPixmap pixmap, bool iconHover) : QObject(parent)
{
    //Constructor MainWindow
    init(hLayout, iconHover);
    setIcon(pixmap);
}


DraftItemCounter::DraftItemCounter(QObject *parent, QGridLayout *gridLayout, int gridRow, int gridCol,
                                   QPixmap pixmap, int iconWidth, bool iconHover) : QObject(parent)
{
    //Constructor DraftMechanicsWindow
    QHBoxLayout *hLayout = new QHBoxLayout();
    init(hLayout, iconHover);
    setTheme(pixmap, iconWidth);

    gridLayout->addLayout(hLayout, gridRow, gridCol);
}


void DraftItemCounter::init(QHBoxLayout *hLayout, bool iconHover)
{
    labelIcon = new HoverLabel();
    labelCounter = new QLabel();
    hLayout->addWidget(labelIcon);
    hLayout->addWidget(labelCounter);

    if(iconHover)
    {
        connect(labelIcon, SIGNAL(enter()),
                this, SLOT(sendIconEnter()));
        connect(labelIcon, SIGNAL(leave()),
                this, SIGNAL(iconLeave()));
    }

    reset();
}


DraftItemCounter::DraftItemCounter(QObject *parent) : QObject(parent)
{
    labelIcon = NULL;
    labelCounter = NULL;
    reset();
}


DraftItemCounter::~DraftItemCounter()
{
    if(labelIcon != NULL)       delete labelIcon;
    if(labelCounter != NULL)    delete labelCounter;
}


void DraftItemCounter::setIcon(QPixmap pixmap, int iconWidth)
{
    labelIcon->setPixmap(pixmap.scaledToWidth(iconWidth, Qt::SmoothTransformation));
}


void DraftItemCounter::setTheme(QPixmap pixmap, int iconWidth)
{
    QFont font(ThemeHandler::defaultFont());
    font.setPixelSize(iconWidth*0.6);
    labelCounter->setFont(font);
    labelCounter->setStyleSheet(".QLabel { color: " + ThemeHandler::fgDraftMechanicsColor() + ";}");

    setIcon(pixmap, iconWidth);
}


void DraftItemCounter::reset()
{
    this->counter = 0;
    this->deckCardList.clear();
    this->deckCardListSyn.clear();

    if(labelIcon != NULL && labelCounter != NULL)
    {
        labelCounter->setText("0");
        labelIcon->setHidden(true);
        labelCounter->setHidden(true);
    }
}


void DraftItemCounter::setTransparency(Transparency transparency, bool mouseInApp)
{
    if(labelIcon != NULL && labelCounter != NULL)
    {
        if(!mouseInApp && transparency == Transparent)
        {
            labelIcon->setStyleSheet("QLabel {background-color: transparent; color: white;}");
            labelCounter->setStyleSheet("QLabel {background-color: transparent; color: white;}");
        }
        else
        {
            labelIcon->setStyleSheet("");
            labelCounter->setStyleSheet("");
        }
    }
}


void DraftItemCounter::increase(int numIncrease, int draftedCardsCount)
{
    if(labelIcon != NULL && labelCounter != NULL)
    {
        if(counter == 0)
        {
            labelIcon->setHidden(false);
            labelCounter->setHidden(false);
        }
        this->counter += numIncrease;
        labelCounter->setText(QString::number((counter*10/std::max(1,draftedCardsCount))/10.0));
    }
}
void DraftItemCounter::increase(const QString &code, bool count)
{
    bool duplicatedCard = false;
    for(DeckCard &deckCard: deckCardList)
    {
        if(deckCard.getCode() == code)
        {
            deckCard.total++;
            deckCard.remaining = deckCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        deckCardList.append(DeckCard(code));
    }

    if(labelIcon != NULL && labelCounter != NULL && count)
    {
        this->counter++;
        labelCounter->setText(QString::number(counter));
        if(counter == 1)
        {
            labelIcon->setHidden(false);
            labelCounter->setHidden(false);
        }
    }
}


void DraftItemCounter::increaseSyn(const QString &code)
{
    bool duplicatedCard = false;
    for(DeckCard &deckCard: deckCardListSyn)
    {
        if(deckCard.getCode() == code)
        {
            deckCard.total++;
            deckCard.remaining = deckCard.total;
            duplicatedCard = true;
            break;
        }
    }

    if(!duplicatedCard)
    {
        deckCardListSyn.append(DeckCard(code));
    }
}


bool DraftItemCounter::insertCode(const QString code, QMap<QString,int> &synergies)
{
    for(DeckCard &deckCard: deckCardList)
    {
        if(code == deckCard.getCode())
        {
            if(!synergies.contains(code))
            {
                synergies[code] = deckCard.total;
            }
            return true;
        }
    }
    return false;
}


void DraftItemCounter::insertCards(QMap<QString,int> &synergies)
{
    for(DeckCard &deckCard: deckCardList)
    {
        QString code = deckCard.getCode();
        if(!synergies.contains(code))
        {
            synergies[code] = deckCard.total;
        }
    }
}


void DraftItemCounter::insertSynCards(QMap<QString,int> &synergies)
{
    for(DeckCard &deckCard: deckCardListSyn)
    {
        QString code = deckCard.getCode();
        if(!synergies.contains(code))
        {
            synergies[code] = deckCard.total;
        }
    }
}


bool DraftItemCounter::isEmpty()
{
    return counter == 0;
}


int DraftItemCounter::count()
{
    return counter;
}


QList<DeckCard> DraftItemCounter::getDeckCardList()
{
    return deckCardList;
}


void DraftItemCounter::sendIconEnter()
{
    QPoint topLeft = labelIcon->mapToGlobal(QPoint(0,0));
    QPoint bottomRight = labelIcon->mapToGlobal(QPoint(labelIcon->width(),labelIcon->height()));
    QRect labelRect = QRect(topLeft, bottomRight);

    QMap<int,DeckCard> deckCardMap;
    for(DeckCard &deckCard: deckCardList)   deckCardMap.insertMulti(deckCard.getCost(), deckCard);
    QList<DeckCard> deckCardOrderedList = deckCardMap.values();

    emit iconEnter(deckCardOrderedList, labelRect);
}













