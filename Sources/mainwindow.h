#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logloader.h"
#include "gamewatcher.h"
#include "Cards/deckcard.h"
#include "hscarddownloader.h"
#include "enemydeckhandler.h"
#include "deckhandler.h"
#include "enemyhandhandler.h"
#include "planhandler.h"
#include "arenahandler.h"
#include "secretshandler.h"
#include "drafthandler.h"
#include "trackobotuploader.h"
#include "Widgets/cardwindow.h"
#include "Widgets/cardlistwindow.h"
#include <QMainWindow>
#include <QJsonObject>

#define DIVIDE_TABS_H 444
#define DIVIDE_TABS_H2 666
#define DIVIDE_TABS_V 500
#define BIG_BUTTONS_H 48
#define SMALL_BUTTONS_H 19
#define DEFAULT_THEME "Purple"
#define JSON_CARDS_URL "https://api.hearthstonejson.com/v1/latest/all/cards.json"
#define LIGHTFORGE_JSON_URL "http://thelightforge.com/api/TierList/Latest?locale=enUS"
#define EXTRA_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Extra"
#define IMAGES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Images"
#define THEMES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Themes"
#define HA_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/HearthArena"
#define SYNERGIES_URL "https://raw.githubusercontent.com/supertriodo/Arena-Tracker/master/Synergies"


namespace Ui {
class Extended;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    explicit MainWindow(QWidget *parent = 0);
    MainWindow(QWidget *parent, MainWindow *primaryWindow);
    ~MainWindow();

private:
    enum WindowsFormation {H1, H2, H3, V2, None};

//Variables
private:
    Ui::Extended *ui;
    LogLoader *logLoader;
    GameWatcher *gameWatcher;
    HSCardDownloader *cardDownloader;
    EnemyDeckHandler *enemyDeckHandler;
    DeckHandler *deckHandler;
    EnemyHandHandler *enemyHandHandler;
    PlanHandler *planHandler;
    ArenaHandler *arenaHandler;
    SecretsHandler *secretsHandler;
    DraftHandler * draftHandler;
    CardWindow *cardWindow;
    CardListWindow *cardListWindow;
    TrackobotUploader *trackobotUploader;
    QMap<QString, QJsonObject> cardsJson;
    QPoint dragPosition;
    WindowsFormation windowsFormation;
    QFile* atLogFile;
    bool splitWindow;
    bool mouseInApp;
    Transparency transparency;
    DraftMethod draftMethod;
    bool oneWindow;
    bool isMainWindow;
    MainWindow *otherWindow;
    int cardHeight;
    int drawDisappear;
    bool showDraftOverlay;
    bool draftLearningMode;
    QString draftLogFile;
    bool copyGameLogs;
    QNetworkAccessManager *networkManager;
    QStringList allCardsDownloadList;



//Metodos
public:
    void calculateMinimumWidth();
    LoadingScreenState getLoadingScreen();
    void resizeTabWidgets();

private:
    void createLogLoader();
    void createArenaHandler();
    void createGameWatcher();
    void createCardWindow();
    void createCardListWindow();
    void createCardDownloader();
    void createEnemyDeckHandler();
    void createDeckHandler();
    void createEnemyHandHandler();
    void createPlanHandler();
    void createSecretsHandler();
    void createDraftHandler();
    void createVersionChecker();
    void createTrackobotUploader();
    void readSettings();
    void writeSettings();
    void completeUI();
    void completeUIButtons();
    void completeConfigTab();
    void addDraftMenu(QPushButton *button);
    void spreadSplitWindow();
    void spreadTransparency(Transparency newTransparency);
    void updateOtherTabsTransparency();
    void spreadTheme(bool redrawAllGames);
    void updateMainUITheme();
    void updateButtonsTheme();
    void updateTabWidgetsTheme(bool transparent, bool resizing);
    QString getHSLanguage();
    void createCardsJsonMap(QByteArray &jsonData);
    void resizeTopButtons(int right, int top);
    void resizeChecks();
    void resizeTabWidgets(QSize newSize);
    void resizeTabWidgets(WindowsFormation newWindowsFormation);
    void moveTabTo(QWidget *widget, QTabWidget *tabWidget);
    void resetSettings();
    void createLogFile();
    void closeLogFile();
    void createSecondaryWindow();
    void destroySecondaryWindow();
    void createDataDir();
    bool createDir(QString pathDir);
    void calculateDeckWindowMinimumWidth();
    void initConfigTab(int tooltipScale, int cardHeight, bool autoSize, bool showClassColor, bool showSpellColor, bool showManaLimits, bool showTotalAttack, bool showRngList, int maxGamesLog, QString theme);
    void moveInScreen(QPoint pos, QSize size);
    int getScreenHighest();
    void completeHighResConfigTab();
    void checkGamesLogDir();
    void checkDraftLogLine(QString logLine, QString file);
    void removeNonCompleteDraft();
    void spreadTamCard(int value);
    int getTamCard();
    int getAutoTamCard();
    void createNetworkManager();
    void initCardsJson();
    void removeHSCards();   //Desactivado: Remove HSCards when upgrading version
    void checkCardsJsonVersion(QString cardsJsonVersion);
    void askLinuxShortcut();
    void createLinuxShortcut();
    void createDebugPack();
    void showWindowFrame(bool showFrame=true);
    void downloadLightForgeJson();
    void spreadDraftMethod(DraftMethod draftMethod);
    void showProgressBar(bool animated=true);
    bool askImportXls();
    bool askImportAccount();
    void checkFirstRunNewVersion();
    void startProgressBarMini(int maximum);
    void hideProgressBarMini();
    void advanceProgressBarMini(int remaining);
    void updateProgressAllCardsDownload(QString code);
    void completeConfigComboTheme();
    void redrawAllGames();
    void initConfigTheme(QString theme);
    void unZip(QString zipName, QString targetPath);
    void downloadExtraFiles();
    void downloadThemes();
    void downloadTheme(QString theme, int version);
    void downloadHearthArenaVersion();
    void downloadHearthArenaJson(int version);
    void downloadSynergiesVersion();
    void downloadSynergiesJson(int version);

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;

//Signals
signals:
    void cardsJsonReady();

//Slots
public slots:
    //GameWatcher
    void resetDeck(bool deckRead=false);
    void resetDeckDontRead();

    //Multi Handlers
    bool checkCardImage(QString code, bool isHero=false);

    //HSCardDownloader
    void redrawDownloadedCardImage(QString code);

    //Widgets
    void resizeSlot(QSize size);

    //MainWindow
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="MainWindow");
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel, QString file);


private slots:
    void test();
    void testPlan();
    void testDelay();
    void testSynergies();
    void confirmNewArenaDraft(QString hero);
    void toggleSplitWindow();
    void transparentAlways();
    void transparentAuto();
    void transparentNever();
    void transparentFramed();
    void toggleDeckWindow();
    void updateTimeDraw(int value);
    void updateTamCard(int value);
    void toggleShowDraftOverlay();
    void toggleDraftLearningMode();
    void updateTooltipScale(int value);
    void closeApp();
    void updateShowClassColor(bool checked);
    void updateShowSpellColor(bool checked);
    void updateShowManaLimits(bool checked);
    void fadeBarAndButtons(bool fadeOut);
    void spreadMouseInApp();
    void updateMaxGamesLog(int value);
    void logReset();
    void spreadCorrectTamCard();
    void completeArenaDeck();
    void resizeChangingTab();
    void updateShowTotalAttack(bool checked);
    void updateShowRngList(bool checked);
    void setLocalLang();
    void replyFinished(QNetworkReply *reply);
    void checkLinuxShortcut();
    void draftMethodHA();
    void draftMethodLF();
    void draftMethodCombined();
    void spreadTransparency();
    void startProgressBar(int maximum, QString text);
    void advanceProgressBar(int remaining, QString text);
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void hideProgressBar();
    void downloadAllArenaCodes();
    void missingOnWeb(QString code);
    void allCardsDownloaded();
    void init();
    void loadTheme(QString theme, bool initTheme=false);
};

#endif // MAINWINDOW_H
