//
// MainWindow.hpp
// NaoCar Remote
//

#ifndef _MAIN_WINDOW_HPP_
# define _MAIN_WINDOW_HPP_

# include <QObject>
# include <QMainWindow>
# include <QTimer>
# include <QPixmap>
# include <QKeyEvent>

# include <vector>

# include "ui_MainWindow.h"

class MainWindowDelegate;

class MainWindow : public QObject {
    Q_OBJECT
public:

    enum Direction {
        Left,
        Right,
        Front
    };
    
    enum Move {
        Frontwards,
        Backwards,
        Stopped
    };
    
    MainWindow(MainWindowDelegate* delegate);
    ~MainWindow(void);
    
    void setStreamImage(QImage* image);
    
    QMainWindow* getWindow(void);
    
    public slots:
    void connectAction(void);
    void disconnectAction(void);
    void hostInputEntered(void);
    void riftAction(void);
    void viewChanged(int index);
    void carambarClicked(void);
    void talkInputEntered(void);
    void autoDrivingClicked(void);
    void safeModeClicked(void);
    
protected:
    bool eventFilter(QObject* obj, QEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    
private:
    MainWindowDelegate*	_delegate;
    QMainWindow		_window;
    Ui_MainWindow		_windowUi;
    Direction		_steeringWheelDirection;
    Move			_move;
};

#endif
