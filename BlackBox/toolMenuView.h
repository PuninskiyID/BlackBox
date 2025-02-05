#ifndef TOOLMENUVIEW_H
#define TOOLMENUVIEW_H

#include <QtCore>
#include <QtWidgets>

class ToolMenuView
{
    QPushButton *resetScale;
    QPushButton *autoScale;
    QPushButton *drawMode;

    QPushButton *startLog;
    QPushButton *openLog;

    QComboBox *motorsCombo;
    QSpinBox *pageSpinBox;

public:
    ToolMenuView();
};

#endif // TOOLMENUVIEW_H
