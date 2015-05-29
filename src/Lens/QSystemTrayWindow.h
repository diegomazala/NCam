#ifndef _Q_SYSTEM_TRAY_WINDOW_H_
#define _Q_SYSTEM_TRAY_WINDOW_H_

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

//! [0]
class QSystemTrayWindow : public QDialog
{
    Q_OBJECT

public:
	QSystemTrayWindow();

    void setVisible(bool visible) Q_DECL_OVERRIDE;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

	QIcon icon;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON

#endif	// _Q_SYSTEM_TRAY_WINDOW_H_
