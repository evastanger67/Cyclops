#ifndef MAIN_VISOR_H
#define MAIN_VISOR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

class MainVisor : public QWidget
{
  Q_OBJECT

public:
  explicit MainVisor(QWidget *parent = nullptr);

private slots:
  void openDatabase();
  void openTableViewer();
  void openQueryExecutor();

private:
  void setupUI();
  void updateDatabaseStatus();

  QPushButton *m_openDbBtn;
  QPushButton *m_tableViewerBtn;
  QPushButton *m_queryExecutorBtn;
  QLabel *m_statusLabel;

  QString m_databasePath;
};

#endif // MAIN_VISOR_H
