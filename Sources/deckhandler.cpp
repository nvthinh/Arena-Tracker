#include "deckhandler.h"
#include <QtWidgets>

DeckHandler::DeckHandler(QObject *parent, QMap<QString, QJsonObject> *cardsJson, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->cardsJson = cardsJson;
    this->inGame = false;
    this->inArena = false;
    this->transparency = Opaque;
    this->greyedHeight = 35;
    this->cardHeight = 35;
    this->drawAnimating = false;
    this->drawDisappear = 10;
    this->synchronized = false;
    this->loadedDeckName = QString();
    this->loadDeckItemsMap.clear();

    completeUI();
}

DeckHandler::~DeckHandler()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
    drawCardList.clear();
}


void DeckHandler::completeUI()
{
    ui->deckButtonMin->setEnabled(false);
    ui->deckButtonPlus->setEnabled(false);
    ui->deckButtonRemove->setEnabled(false);
    hideDeckButtons();

    ui->deckButtonSave->setEnabled(false);

    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->drawListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->drawListWidget->setMouseTracking(true);
    ui->deckListWidget->setMouseTracking(true);

    createDeckTreeWidget();

    connect(ui->deckListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(enableDeckButtons()));
    connect(ui->deckListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDeckCardEntered(QListWidgetItem*)));
    connect(ui->deckListWidget, SIGNAL(xLeave()),
            this, SLOT(deselectRow()));
    connect(ui->drawListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findDrawCardEntered(QListWidgetItem*)));
    connect(ui->deckButtonMin, SIGNAL(clicked()),
            this, SLOT(cardTotalMin()));
    connect(ui->deckButtonPlus, SIGNAL(clicked()),
            this, SLOT(cardTotalPlus()));
    connect(ui->deckButtonRemove, SIGNAL(clicked()),
            this, SLOT(cardRemove()));

    connect(ui->deckLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(enableDeckButtonSave()));
    connect(ui->deckButtonSave, SIGNAL(clicked()),
            this, SLOT(saveDeck()));
    connect(ui->deckButtonNew, SIGNAL(clicked()),
            this, SLOT(newDeck()));
    connect(ui->deckButtonLoad, SIGNAL(clicked()),
            this, SLOT(toggleLoadDeckTreeWidget()));
}


void DeckHandler::createDeckTreeWidget()
{
    QTreeWidget *treeWidget = ui->loadDeckTreeWidget;
    treeWidget->setColumnCount(1);
    treeWidget->setIconSize(QSize(32,32));
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setHidden(true);
    treeWidget->setFixedHeight(0);

    for(int i=0; i<9; i++)
    {
        loadDeckClasses[i] = new QTreeWidgetItem(treeWidget);
        loadDeckClasses[i]->setHidden(true);
        loadDeckClasses[i]->setExpanded(true);
        loadDeckClasses[i]->setText(0, Utility::getHeroName(i));
        loadDeckClasses[i]->setIcon(0, QIcon(":/Images/hero" + Utility::getHeroLogNumber(i) + ".png"));
        loadDeckClasses[i]->setForeground(0, QBrush(QColor(Utility::getHeroColor(i))));
    }

    loadDeckClasses[9] = new QTreeWidgetItem(treeWidget);
    loadDeckClasses[9]->setHidden(true);
    loadDeckClasses[9]->setExpanded(true);
    loadDeckClasses[9]->setText(0, "Multi class");
    loadDeckClasses[9]->setIcon(0, QIcon(":/Images/secretHunter.png"));
    loadDeckClasses[9]->setForeground(0, QBrush(QColor(Utility::getHeroColor(9))));

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(unselectClassItems()));
    connect(treeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(hideDeckTreeWidget()));
}


bool DeckHandler::isItemClass(QTreeWidgetItem *item)
{
    for(int i=0; i<10; i++) if(item == loadDeckClasses[i])  return true;
    return false;
}


void DeckHandler::unselectClassItems()
{
    if(ui->loadDeckTreeWidget->selectedItems().isEmpty())   return;
    QTreeWidgetItem *item = ui->loadDeckTreeWidget->selectedItems().first();
    if(isItemClass(item))   ui->loadDeckTreeWidget->clearSelection();
}


void DeckHandler::loadSelectedDeck()
{
    if(ui->loadDeckTreeWidget->selectedItems().isEmpty())   return;
    QTreeWidgetItem *item = ui->loadDeckTreeWidget->selectedItems().first();
    if(item!=NULL && !isItemClass(item))  loadDeck(item->text(0));
}


void DeckHandler::toggleLoadDeckTreeWidget()
{
    if(ui->loadDeckTreeWidget->isHidden())  showDeckTreeWidget();
    else                                    hideDeckTreeWidget();
}


void DeckHandler::showDeckTreeWidget()
{
    ui->deckButtonLoad->setEnabled(false);
    ui->loadDeckTreeWidget->setHidden(false);
    ui->loadDeckTreeWidget->clearSelection();
    int totalHeight = ui->deckListWidget->height();
    QEasingCurve easingCurve = QEasingCurve::OutCubic;

    //Muestra
    QPropertyAnimation *animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start();

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishShowDeckTreeWidget()));

    animation = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->loadDeckTreeWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start();

    //Oculta
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(easingCurve);
    animation2->start();
}


void DeckHandler::finishShowDeckTreeWidget()
{
    ui->deckListWidget->setHidden(true);
    ui->deckListWidget->setFixedHeight(0);
    ui->loadDeckTreeWidget->setMinimumHeight(0);
    ui->loadDeckTreeWidget->setMaximumHeight(16777215);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::hideDeckTreeWidget()
{
    loadSelectedDeck();
    ui->deckButtonLoad->setEnabled(false);
    ui->deckListWidget->setHidden(false);
    int totalHeight = ui->loadDeckTreeWidget->height();
    QEasingCurve easingCurve = QEasingCurve::InCubic;

    //Muestra
    QPropertyAnimation *animation = new QPropertyAnimation(ui->deckListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start();

    connect(animation, SIGNAL(finished()),
            this, SLOT(finishHideDeckTreeWidget()));

    animation = new QPropertyAnimation(ui->deckListWidget, "maximumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->deckListWidget->minimumHeight());
    animation->setEndValue(totalHeight);
    animation->setEasingCurve(easingCurve);
    animation->start();

    //Oculta
    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->loadDeckTreeWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(totalHeight);
    animation2->setEndValue(0);
    animation2->setEasingCurve(easingCurve);
    animation2->start();
}


void DeckHandler::finishHideDeckTreeWidget()
{
    ui->loadDeckTreeWidget->setHidden(true);
    ui->loadDeckTreeWidget->setFixedHeight(0);
    ui->deckListWidget->setMinimumHeight(0);
    ui->deckListWidget->setMaximumHeight(16777215);
    ui->deckButtonLoad->setEnabled(true);
}


void DeckHandler::enableDeckButtonSave()
{
    ui->deckButtonSave->setEnabled(true);
}


void DeckHandler::setSynchronized()
{
    this->synchronized = true;

    if(this->inGame)    lockDeckInterface();
    else                unlockDeckInterface();
}


void DeckHandler::adjustDrawSize()
{
    if(drawAnimating)
    {
        QTimer::singleShot(ANIMATION_TIME+50, this, SLOT(adjustDrawSize()));
        return;
    }

    int rowHeight = ui->drawListWidget->sizeHintForRow(0);
    int rows = drawCardList.count();
    int height = rows*rowHeight + 2*ui->drawListWidget->frameWidth();
    int maxHeight = (ui->drawListWidget->height()+ui->enemyHandListWidget->height())*4/5;
    if(height>maxHeight)    height = maxHeight;

    QPropertyAnimation *animation = new QPropertyAnimation(ui->drawListWidget, "minimumHeight");
    animation->setDuration(ANIMATION_TIME);
    animation->setStartValue(ui->drawListWidget->minimumHeight());
    animation->setEndValue(height);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();

    QPropertyAnimation *animation2 = new QPropertyAnimation(ui->drawListWidget, "maximumHeight");
    animation2->setDuration(ANIMATION_TIME);
    animation2->setStartValue(ui->drawListWidget->maximumHeight());
    animation2->setEndValue(height);
    animation2->setEasingCurve(QEasingCurve::OutBounce);
    animation2->start();

    this->drawAnimating = true;
    connect(animation, SIGNAL(finished()),
            this, SLOT(clearDrawAnimating()));
}


void DeckHandler::clearDrawAnimating()
{
    this->drawAnimating = false;
    if(drawCardList.empty())    ui->drawListWidget->setHidden(true);
}


void DeckHandler::reset()
{
    ui->deckListWidget->clear();
    deckCardList.clear();
    clearDrawList(true);

    DeckCard deckCard("");
    deckCard.total = 30;
    deckCard.listItem = new QListWidgetItem();
    deckCard.draw(true, this->cardHeight);
    insertDeckCard(deckCard);

    enableDeckButtons();

    emit pDebug("Deck list cleared.");
}


QList<DeckCard> * DeckHandler::getDeckComplete()
{
    if(deckCardList[0].total==0)    return &deckCardList;
    else    return NULL;
}


void DeckHandler::newDeckCardAsset(QString code)
{
    newDeckCard(code, 1, true);
}


void DeckHandler::newDeckCardDraft(QString code)
{
    newDeckCard(code, 1, true);
}


void DeckHandler::newDeckCardWeb(QString code, int total)
{
    newDeckCard(code, total, false);
}


void DeckHandler::newDeckCard(QString code, int total, bool add)
{
    if(code.isEmpty())  return;

    //Mazo completo
    if(deckCardList[0].total < (uint)total)
    {
        emit pDebug("Deck is full: Not adding: (" + QString::number(total) + ") " +
                    (*cardsJson)[code].value("name").toString(), Warning);
        return;
    }

    //Ya existe en el mazo
    bool found = false;
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCardList[i].getCode() == code)
        {
            if(!add)
            {
                emit pDebug((*cardsJson)[code].value("name").toString() + " already in deck.");
                return;
            }

            found = true;
            deckCardList[i].total+=total;
            deckCardList[i].remaining+=total;
            deckCardList[i].draw(true, this->cardHeight);
            break;
        }
    }

    if(!found)
    {
        DeckCard deckCard(code);
        deckCard.total = total;
        deckCard.remaining = total;
        deckCard.listItem = new QListWidgetItem();
        insertDeckCard(deckCard);
        deckCard.draw(true, this->cardHeight);
        emit checkCardImage(code);
    }

    deckCardList[0].total-=total;
    deckCardList[0].draw(true, this->cardHeight);
    if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);

    enableDeckButtonSave();

    emit pDebug("Add to deck: (" + QString::number(total) + ")" +
                (*cardsJson)[code].value("name").toString());
}


void DeckHandler::insertDeckCard(DeckCard &deckCard)
{
    for(int i=0; i<deckCardList.length(); i++)
    {
        if(deckCard.getCost() < deckCardList[i].getCost())
        {
            deckCardList.insert(i, deckCard);
            ui->deckListWidget->insertItem(i, deckCard.listItem);
            return;
        }
        else if(deckCard.getCost() == deckCardList[i].getCost())
        {
            if(deckCard.getType() != deckCardList[i].getType())
            {
                if(deckCard.getType() == "Weapon" || deckCardList[i].getType() == "Minion")
                {
                    deckCardList.insert(i, deckCard);
                    ui->deckListWidget->insertItem(i, deckCard.listItem);
                    return;
                }
            }
            else if(deckCard.getName().toLower() < deckCardList[i].getName().toLower())
            {
                deckCardList.insert(i, deckCard);
                ui->deckListWidget->insertItem(i, deckCard.listItem);
                return;
            }
        }
    }
    deckCardList.append(deckCard);
    ui->deckListWidget->addItem(deckCard.listItem);
}


void DeckHandler::newDrawCard(QString code)
{
    DrawCard drawCard(code);
    drawCard.listItem = new QListWidgetItem();
    drawCardList.append(drawCard);
    ui->drawListWidget->addItem(drawCard.listItem);
    drawCard.draw();
    emit checkCardImage(code);
    ui->drawListWidget->setHidden(false);
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));

    if(this->drawDisappear>0)   QTimer::singleShot(this->drawDisappear*1000,
                                                    this, SLOT(removeOldestDrawCard()));
}


void DeckHandler::removeOldestDrawCard()
{
    if(drawCardList.empty())    return;
    DrawCard drawCard = drawCardList.takeFirst();
    ui->drawListWidget->removeItemWidget(drawCard.listItem);
    delete drawCard.listItem;
    QTimer::singleShot(10, this, SLOT(adjustDrawSize()));
}


void DeckHandler::showPlayerCardDraw(QString code)
{
    if(this->drawDisappear>=0)    newDrawCard(code);
    drawFromDeck(code);
}


void DeckHandler::drawFromDeck(QString code)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining>1)
            {
                it->remaining--;
                it->draw(false, this->cardHeight);
            }
            else if(it->remaining == 1)
            {
                it->remaining--;
                it->drawGreyed(true, this->greyedHeight);
            }
            //it->remaining == 0
            //MALORNE
            else if(code == MALORNE)  return;
            //Reajustamos el mazo si tiene unknown cards
            else if(deckCardList[0].total>0)
            {
                deckCardList[0].total--;
                if(deckCardList[0].total == 0)  deckCardList[0].listItem->setHidden(true);
                else                            deckCardList[0].draw(true, this->cardHeight);
                it->total++;

                it->drawGreyed(true, this->greyedHeight);

                emit pDebug("New card: " + it->getName());
//                emit pLog(tr("Deck: New card: ") + it->getName());
            }
            else
            {
                emit pDebug("New card but deck is full. " + it->getName(), Warning);
//                emit pLog(tr("Deck: New card found but deck is full: ") + it->getName());
            }
            return;
        }
    }

    //MALORNE
    if(code == MALORNE)  return;


    if(deckCardList[0].total>0)
    {
        emit pDebug("New card: " +
                          (*cardsJson)[code].value("name").toString());
//        emit pLog(tr("Deck: New card: ") +
//                          (*cardsJson)[code].value("name").toString());
        newDeckCard(code);
        drawFromDeck(code);
    }
    else
    {
        emit pDebug("New card but deck is full. " +
                      (*cardsJson)[code].value("name").toString(), Warning);
//        emit pLog(tr("Deck: New card found but deck is full: ") +
//                      (*cardsJson)[code].value("name").toString());
    }
}


void DeckHandler::redrawDownloadedCardImage(QString code)
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            if(it->remaining > 0)
            {
                it->draw(false, this->cardHeight);
            }
            else
            {
                it->drawGreyed(true, this->greyedHeight);
            }
        }
    }

    for (QList<DrawCard>::iterator it = drawCardList.begin(); it != drawCardList.end(); it++)
    {
        if(it->getCode() == code)
        {
            it->draw();
        }
    }
}


void DeckHandler::enableDeckButtons()
{
    int index = ui->deckListWidget->currentRow();

    if(index>0 && deckCardList[index].total > 1)
                                        ui->deckButtonMin->setEnabled(true);
    else                                ui->deckButtonMin->setEnabled(false);
    if(index>0 && deckCardList[index].total == 1)
                                        ui->deckButtonRemove->setEnabled(true);
    else                                ui->deckButtonRemove->setEnabled(false);
    if(index>0 && deckCardList.first().total > 0 &&
            (inArena |
                ((deckCardList[index].total == 1) && (deckCardList[index].getRarity() != "Legendary"))
            )
        )
                                        ui->deckButtonPlus->setEnabled(true);
    else                                ui->deckButtonPlus->setEnabled(false);


    if(index>0)
    {
        showDeckButtons();
    }
    else
    {
        hideDeckButtons();
    }
}


void DeckHandler::showDeckButtons()
{
    if(ui->deckButtonMin->isHidden())
    {
        ui->tabDeckLayout->removeItem(ui->horizontalLayoutDeckButtons);
        ui->tabDeckLayout->insertItem(1, ui->horizontalLayoutDeckButtons);
        ui->deckButtonMin->setHidden(false);
        ui->deckButtonPlus->setHidden(false);
        ui->deckButtonRemove->setHidden(false);
    }
}


void DeckHandler::hideDeckButtons()
{
    ui->deckButtonMin->setHidden(true);
    ui->deckButtonPlus->setHidden(true);
    ui->deckButtonRemove->setHidden(true);
    ui->tabDeckLayout->removeItem(ui->horizontalLayoutDeckButtons);
}


void DeckHandler::deselectRow()
{
    ui->deckListWidget->setCurrentRow(-1);
}


void DeckHandler::cardTotalMin()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total--;
    deckCardList[index].remaining = deckCardList[index].total;
    deckCardList[0].total++;

    deckCardList[index].draw(true, this->cardHeight);
    if(deckCardList[0].total==1)    deckCardList[0].listItem->setHidden(false);
    deckCardList[0].draw(true, this->cardHeight);
    enableDeckButtons();

    enableDeckButtonSave();
}


void DeckHandler::cardTotalPlus()
{
    int index = ui->deckListWidget->currentRow();
    deckCardList[index].total++;
    deckCardList[index].remaining = deckCardList[index].total;
    deckCardList[0].total--;

    deckCardList[index].draw(true, this->cardHeight);
    if(deckCardList[0].total==0)    deckCardList[0].listItem->setHidden(true);
    else                            deckCardList[0].draw(true, this->cardHeight);
    enableDeckButtons();

    enableDeckButtonSave();
}


void DeckHandler::cardRemove()
{
    int index = ui->deckListWidget->currentRow();
    if(deckCardList[index].total!=1 || index==0)
    {
        enableDeckButtons();
        return;
    }

//    int ret = QMessageBox::warning(ui->centralWidget, tr("Sure?"), tr("Remove (") +
//            deckCardList[index].getName() +   tr(") from your deck?"),
//            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
//    if(ret == QMessageBox::Cancel)  return;

    ui->deckListWidget->removeItemWidget(deckCardList[index].listItem);
    delete deckCardList[index].listItem;
    deckCardList.removeAt(index);

    deckCardList[0].total++;
    if(deckCardList[0].total==1)    deckCardList[0].listItem->setHidden(false);
    deckCardList[0].draw(true, this->cardHeight);
    enableDeckButtons();

    enableDeckButtonSave();
}


void DeckHandler::lockDeckInterface()
{
    this->inGame = true;
    if(!synchronized)   return;

    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        it->remaining = it->total;
    }

    ui->deckListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->deckListWidget->selectionModel()->reset();
    ui->deckListWidget->clearFocus();
    ui->deckListWidget->setFocusPolicy(Qt::NoFocus);
    hideDeckButtons();
    hideManageDecksButtons();

    updateTransparency();
    clearDrawList(true);

    emit pDebug("Lock deck interface.");
}


void DeckHandler::unlockDeckInterface()
{
    this->inGame = false;
    if(!synchronized)   return;

    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->total>0)
        {
            it->draw(true, this->cardHeight);
            it->listItem->setHidden(false);
            it->remaining = it->total;
        }
        else    it->listItem->setHidden(true);
    }

    ui->deckListWidget->setFocusPolicy(Qt::ClickFocus);
    ui->deckListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    showManageDecksButtons();

    updateTransparency();
    clearDrawList(true);

    emit pDebug("Unlock deck interface.");
}


void DeckHandler::updateTransparency()
{
    if(transparency==Transparent || (inGame && transparency==AutoTransparent))
    {
        ui->tabDeck->setAttribute(Qt::WA_NoBackground);
        ui->tabDeck->repaint();
    }
    else
    {
        ui->tabDeck->setAttribute(Qt::WA_NoBackground, false);
        ui->tabDeck->repaint();
    }
}


void DeckHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void DeckHandler::updateGreyedHeight()
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->remaining == 0)
        {
            it->drawGreyed(true, this->greyedHeight);
        }
    }
}


void DeckHandler::setGreyedHeight(int value)
{
    this->greyedHeight = value;
    if(inGame)  updateGreyedHeight();
}


void DeckHandler::updateCardHeight()
{
    for (QList<DeckCard>::iterator it = deckCardList.begin(); it != deckCardList.end(); it++)
    {
        if(it->remaining > 0)
        {
            it->draw(true, this->cardHeight);
        }
    }
}


void DeckHandler::setCardHeight(int value)
{
    this->cardHeight = value;
    updateCardHeight();
}


void DeckHandler::clearDrawList(bool forceClear)
{
    if(!forceClear && this->drawDisappear>0)     return;

    ui->drawListWidget->clear();
    ui->drawListWidget->setHidden(true);
    ui->drawListWidget->setMinimumHeight(0);
    ui->drawListWidget->setMaximumHeight(0);
    drawCardList.clear();
}


void DeckHandler::setDrawDisappear(int value)
{
    this->drawDisappear = value;
    clearDrawList(true);
}


void DeckHandler::findDeckCardEntered(QListWidgetItem * item)
{
    QString code = deckCardList[ui->deckListWidget->row(item)].getCode();

    QRect rectCard = ui->deckListWidget->visualItemRect(item);
    QPoint posCard = ui->deckListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int deckListTop = ui->deckListWidget->mapToGlobal(QPoint(0,0)).y();
    int deckListBottom = ui->deckListWidget->mapToGlobal(QPoint(0,ui->deckListWidget->height())).y();
    emit cardEntered(code, globalRectCard, deckListTop, deckListBottom);
}


void DeckHandler::findDrawCardEntered(QListWidgetItem * item)
{
    QString code = drawCardList[ui->drawListWidget->row(item)].getCode();

    QRect rectCard = ui->drawListWidget->visualItemRect(item);
    QPoint posCard = ui->drawListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int drawListTop = -1;
    int drawListBottom = ui->drawListWidget->mapToGlobal(QPoint(0,ui->drawListWidget->height())).y();
    emit cardEntered(code, globalRectCard, drawListTop, drawListBottom);
}


/*
 * ObjectDecks
 * {
 * deck1 -> ObjectDeck
 * deck2 -> ObjectDeck
 * }
 *
 * ObjectDeck
 * {
 * id1 -> nCards
 * id2 -> nCards
 * hero -> heroLog
 * }
 */
void DeckHandler::loadDecks()
{
    QFile jsonFile(Utility::appPath() + "/HSCards/ArenaTrackerDecks.json");
    if(!jsonFile.exists())
    {
        emit pDebug("Json decks file doesn't exists.");
        emit pLog("Deck: Loaded 0 decks.");
        return;
    }

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pDebug("Failed to load Arena Tracker decks json from disk.", Error);
        emit pLog(tr("File: ERROR: Loading Arena Tracker decks json from disk. Make sure HSCards dir is in the same place as the exe."));
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    decksJson = jsonDoc.object();

    emit pDebug("Loaded " + QString::number(decksJson.count()) + " decks from json file.");
    emit pLog("Deck: Loaded " + QString::number(decksJson.count()) + " decks.");

    //Iniciamos deckCardList con 30 cartas desconocidas
    ui->deckButtonSave->setEnabled(false);
    newDeck();

    //Load decks to loadDeckTreeWidget
    foreach(QString deckName, decksJson.keys())
    {
        addDeckToLoadTree(deckName);
    }
}


void DeckHandler::addDeckToLoadTree(QString deckName)
{
    if(!decksJson.contains(deckName))
    {
        emit pDebug("Deck " + deckName + " not found. Adding to loadDeckTreeWidget.", Error);
        return;
    }

    int indexClassArray[9] = {8,6,5,3,1,0,7,2,4};
    QString heroLog = decksJson[deckName].toObject()["hero"].toString();
    int numberClass = heroLog.toInt()-1;
    int indexClass;
    if(numberClass<0 || numberClass>8)      indexClass = 9;
    else                                    indexClass = indexClassArray[numberClass];
    QTreeWidgetItem *deckClass = loadDeckClasses[indexClass];
    if(deckClass->isHidden())   deckClass->setHidden(false);

    //Create item
    QTreeWidgetItem *item = new QTreeWidgetItem(deckClass);
    item->setText(0, deckName);
    item->setForeground(0, QBrush(QColor(Utility::getHeroColor(indexClass))));
    loadDeckItemsMap[deckName] = item;
}


void DeckHandler::loadDeck(QString deckName)
{
    if(ui->deckButtonSave->isEnabled() && !askSaveDeck())   return;

    if(!decksJson.contains(deckName))
    {
        emit pDebug("Deck " + deckName + " not found. Trying to load.", Error);
        return;
    }

    reset();
    QJsonObject jsonObjectDeck = decksJson[deckName].toObject();

    foreach(QString key, jsonObjectDeck.keys())
    {
        if(key != "hero")   newDeckCard(key, jsonObjectDeck[key].toInt());
    }

    loadedDeckName = deckName;
    ui->deckLineEdit->setText(deckName);
    ui->deckButtonSave->setEnabled(false);

    emit pDebug("Deck " + deckName + " loaded.");
    emit pLog("Deck: " + deckName + " loaded.");
}


void DeckHandler::saveDeck()
{
    //Create json deck
    QJsonObject jsonObjectDeck;
    QString hero = "";

    foreach(DeckCard deckCard, deckCardList)
    {
        QString code = deckCard.getCode();
        int total = deckCard.total;
        if(!code.isEmpty() && total > 0)
        {
            jsonObjectDeck.insert(deckCard.getCode(), (int)deckCard.total);
            if(hero.isEmpty())
            {
                hero = (*cardsJson)[deckCard.getCode()].value("playerClass").toString();
            }
        }
    }
    jsonObjectDeck.insert("hero", Utility::heroToLogNumber(hero));

    QString deckName = ui->deckLineEdit->text();
    if(!loadedDeckName.isNull())
    {
        decksJson.remove(loadedDeckName);
        emit pDebug("Removed " + loadedDeckName + " from decksJson.");
    }
    decksJson.insert(deckName, jsonObjectDeck);
    loadedDeckName = deckName;
    ui->deckButtonSave->setEnabled(false);

    emit pDebug("Added " + deckName + " to decksJson.");
    emit pLog("Deck: " + deckName + " saved.");

    saveDecksJsonFile();
}


void DeckHandler::saveDecksJsonFile()
{
    //Build json data from decksJson
    QJsonDocument jsonDoc;
    jsonDoc.setObject(decksJson);


    //Save to disk
    QFile jsonFile(Utility::appPath() + "/HSCards/ArenaTrackerDecks.json");
    if(jsonFile.exists())   jsonFile.remove();

    if(!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pDebug("Failed to create Arena Tracker decks json on disk.", Error);
        emit pLog(tr("File: ERROR: Creating Arena Tracker decks json on disk. Make sure HSCards dir is in the same place as the exe."));
        return;
    }
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    emit pDebug("Decks json file updated.");
}


QString DeckHandler::getNewDeckName()
{
    QString newDeckName = "New deck";
    if(!decksJson.contains(newDeckName))    return newDeckName;

    int num = 2;
    while(decksJson.contains(newDeckName + " " + QString::number(num)))    num++;
    return newDeckName + " " + QString::number(num);
}


bool DeckHandler::askSaveDeck()
{
    QString deckName = ui->deckLineEdit->text();

    int ret = QMessageBox::warning(ui->tabDeck, "Save " + deckName + "?",
            deckName + " has unsaved changes.",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

    if(ret == QMessageBox::Save)
    {
        saveDeck();
        return true;
    }
    else if(ret == QMessageBox::Discard)    return true;
    else                                    return false;
}


void DeckHandler::newDeck()
{
    if(ui->deckButtonSave->isEnabled() && !askSaveDeck())   return;

    reset();
    loadedDeckName = QString();
    ui->deckButtonSave->setEnabled(false);
    ui->deckLineEdit->setText(getNewDeckName());
}


void DeckHandler::enterArena()
{
    this->inArena = true;
    ui->deckButtonSave->setEnabled(false);
    hideManageDecksButtons();
}


void DeckHandler::leaveArena()
{
    this->inArena = false;
    ui->deckButtonSave->setEnabled(false);
    showManageDecksButtons();

    //Recuperamos deck
    if(!loadedDeckName.isNull() && decksJson.contains(loadedDeckName))
    {
        loadDeck(loadedDeckName);
    }
    else
    {
        newDeck();
    }
}


void DeckHandler::showManageDecksButtons()
{
    if(ui->deckButtonNew->isHidden() && !inArena && !inGame)
    {
        ui->tabDeckLayout->removeItem(ui->verticalLayoutManageDecks);
        ui->tabDeckLayout->addItem(ui->verticalLayoutManageDecks);
        ui->deckLineEdit->setHidden(false);
        ui->deckButtonNew->setHidden(false);
        ui->deckButtonLoad->setHidden(false);
        ui->deckButtonSave->setHidden(false);
        ui->deckButtonDeleteDeck->setHidden(false);
    }
}


void DeckHandler::hideManageDecksButtons()
{
    ui->deckLineEdit->setHidden(true);
    ui->deckButtonNew->setHidden(true);
    ui->deckButtonLoad->setHidden(true);
    ui->deckButtonSave->setHidden(true);
    ui->deckButtonDeleteDeck->setHidden(true);
    ui->tabDeckLayout->removeItem(ui->verticalLayoutManageDecks);
}

