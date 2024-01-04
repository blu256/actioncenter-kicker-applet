// actioncenter_applet.cpp

#include <tdeglobal.h>
#include <tdelocale.h>
#include <tdeapplication.h>
#include <tqpixmap.h> 
#include "actioncenter_applet.h"
#include <krun.h>
#include <tqdialog.h>
#include <tqlayout.h>
#include <tqlabel.h>
#include <tqsplitter.h>
#include <kprocess.h>

actioncenter_applet::actioncenter_applet(const TQString& configFile, Type type, int actions, TQWidget *parent, const char *name)
    : KPanelApplet(configFile, type, actions, parent, name),
      button1State(false),
      button2State(false),
      splitter(nullptr)
{
    TQPixmap icon("/opt/trinity/share/apps/kicker/pics/actioncenter.png");
    TQPushButton *iconButton = new TQPushButton("", this);
    iconButton->setIcon(icon);
    iconButton->setIconSet(icon);
    iconButton->setFixedSize(icon.size());
    setFixedSize(icon.size().width() + 10, icon.size().height());
    connect(iconButton, SIGNAL(clicked()), this, SLOT(iconClicked()));
    mainView = iconButton;
    mainView->show();
}

actioncenter_applet::~actioncenter_applet()
{
}


void actioncenter_applet::iconClicked()
{
    if (customDialog && customDialog->isVisible()) {
        customDialog->close();
        return;
    }
    customDialog = new TQDialog(this);
    customDialog->setFixedSize(400, getScreenHeight());
    TQVBoxLayout *mainLayout = new TQVBoxLayout(customDialog);

    FILE *notifScript = popen("/opt/trinity/share/apps/actioncenter_applet/notif.sh", "r");
    TQString notifText;
    if (notifScript) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), notifScript) != nullptr) {
            notifText += TQString(buffer).stripWhiteSpace() + "\n";
        }
        pclose(notifScript);
    }

    TQLabel *textLabel = new TQLabel(notifText, customDialog);
    textLabel->setAlignment(TQt::AlignHCenter | TQt::AlignVCenter);
    TQFont font = textLabel->font();
    font.setBold(true);
    textLabel->setFont(font);

    int textHeight = (customDialog->height() * 0.67);
    textLabel->setFixedHeight(textHeight);

    mainLayout->addWidget(textLabel);

    splitter = new TQSplitter(customDialog);
    mainLayout->addWidget(splitter, 0, TQt::AlignHCenter | TQt::AlignVCenter);

FILE* pipe1 = popen("/opt/trinity/share/apps/actioncenter_applet/action1.sh check", "r");
if (pipe1) {
    char buffer[128];
    fgets(buffer, sizeof(buffer), pipe1);
    pclose(pipe1);
    button1State = (TQString(buffer).stripWhiteSpace() == "0");
}

FILE* pipe2 = popen("/opt/trinity/share/apps/actioncenter_applet/action2.sh check", "r");
if (pipe2) {
    char buffer[128];
    fgets(buffer, sizeof(buffer), pipe2);
    pclose(pipe2);
    button2State = (TQString(buffer).stripWhiteSpace() == "0");
}

addImageButton("/opt/trinity/share/apps/actioncenter_applet/action1.png", "/opt/trinity/share/apps/actioncenter_applet/action1_on.png", SLOT(button1Clicked()), button1State);
addImageButton("/opt/trinity/share/apps/actioncenter_applet/action2.png", "/opt/trinity/share/apps/actioncenter_applet/action2_on.png", SLOT(button2Clicked()), button2State);
addButton("/opt/trinity/share/apps/actioncenter_applet/action3.png", SLOT(button3Clicked()));
addButton("/opt/trinity/share/apps/actioncenter_applet/action4.png", SLOT(button4Clicked()));

    int screenWidth = TDEApplication::desktop()->width();
    int dialogWidth = customDialog->width();
    int xPosition = screenWidth - dialogWidth;
    int screenHeight = TDEApplication::desktop()->height();
    int dialogHeight = customDialog->height();
    int yPosition = screenHeight - dialogHeight;
    customDialog->move(xPosition, yPosition);
    customDialog->setCaption("Action center");
    customDialog->show();
}

void actioncenter_applet::addImageButton(const char *imagePathOn, const char *imagePathOff, const char *slot, bool &buttonState)
{
    const char *imagePath = buttonState ? imagePathOff : imagePathOn;
    TQPixmap buttonIcon(imagePath);
    TQPushButton *button = new TQPushButton("", splitter);
    button->setPixmap(buttonIcon);
    button->setFixedSize(buttonIcon.size());
    connect(button, SIGNAL(clicked()), this, slot);
}

void actioncenter_applet::addButton(const char *imagePath, const char *slot)
{
    TQPixmap buttonIcon(imagePath);
    TQPushButton *button = new TQPushButton("", splitter);
    button->setPixmap(buttonIcon);
    button->setFixedSize(buttonIcon.size());
    connect(button, SIGNAL(clicked()), this, slot);
}



void actioncenter_applet::button1Clicked()
{
    KRun::runCommand("/opt/trinity/share/apps/actioncenter_applet/action1.sh");
    button1State = !button1State;
    addImageButton("/opt/trinity/share/apps/actioncenter_applet/action1.png", "/opt/trinity/share/apps/actioncenter_applet/action1_on.png", SLOT(button1Clicked()), button1State);
}

void actioncenter_applet::button2Clicked()
{
    KRun::runCommand("/opt/trinity/share/apps/actioncenter_applet/action2.sh");
    button2State = !button2State;
    addImageButton("/opt/trinity/share/apps/actioncenter_applet/action2.png", "/opt/trinity/share/apps/actioncenter_applet/action2_on.png", SLOT(button2Clicked()), button2State);
}

void actioncenter_applet::button3Clicked()
{
 customDialog->close();
    KRun::runCommand("/opt/trinity/share/apps/actioncenter_applet/action3.sh");
}

void actioncenter_applet::button4Clicked()
{
 customDialog->close();
    KRun::runCommand("/opt/trinity/share/apps/actioncenter_applet/action4.sh");
}

int actioncenter_applet::widthForHeight(int height) const
{
    return width();
}

int actioncenter_applet::heightForWidth(int width) const
{
    return getScreenHeight();
}

void actioncenter_applet::resizeEvent(TQResizeEvent *e)
{
}

int actioncenter_applet::getScreenHeight() const
{
    return TDEApplication::desktop()->height();
}


extern "C"
{
    KPanelApplet* init(TQWidget *parent, const TQString& configFile)
    {
        TDEGlobal::locale()->insertCatalogue("actioncenter_applet");
        return new actioncenter_applet(configFile, KPanelApplet::Normal, 0, parent, "actioncenter_applet");
    }
}