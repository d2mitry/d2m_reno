#ifndef REC_H
#define REC_H

#include <QWidget>
#include <QFileDialog>

namespace Ui {
    class rec;
}

class rec : public QWidget
{
    Q_OBJECT

signals:
    void applyRectmpfile(const QString &tmpfile);
public:
    explicit rec(QWidget *parent = 0);

    void SetrecYes();
    bool GetrecYes();
    ~rec();
private slots:
    void openDir();
    void ApplyClick();
private:
    bool recYes;
    Ui::rec *ui;
    void initButtonConnections();
};

#endif // REC_H
