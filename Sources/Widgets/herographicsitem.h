#ifndef HEROGRAPHICSITEM_H
#define HEROGRAPHICSITEM_H

#include "miniongraphicsitem.h"
#include "../Cards/secretcard.h"


class HeroGraphicsItem : public MinionGraphicsItem
{
    class SecretIcon
    {
    public:
        int id;
        QString code;
        SecretHero secretHero;
    };

//Constructor
public:
    HeroGraphicsItem(QString code, int id, bool friendly, bool playerTurn);
    HeroGraphicsItem(HeroGraphicsItem *copy);

//Variables:
private:
    int armor;
    QList<SecretIcon> secretsList;

public:
    static const int WIDTH = 230;
    static const int HEIGHT = 184;

//Metodos
public:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    bool processTagChange(QString tag, QString value);
    void addSecret(int id, SecretHero secretHero);
    void removeSecret(int id);
    void showSecret(int id, QString code);
    SecretHero getSecretHero(int id);
};

#endif // HEROGRAPHICSITEM_H
