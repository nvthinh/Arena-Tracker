#ifndef HOVERLABEL_H
#define HOVERLABEL_H

#include <QObject>
#include <QLabel>

class HoverLabel : public QLabel
{
    Q_OBJECT

public:
    HoverLabel(QWidget *parent = 0);

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void enter();
    void leave();
};

#endif // HOVERLABEL_H
