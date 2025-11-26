#ifndef VISOR_H
#define VISOR_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QComboBox>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSqlError>
#include <QMessageBox>
class Visor : public QWidget
{
  Q_OBJECT

public:
  explicit Visor(QString database_path_,QWidget *parent = nullptr);
  ~Visor();

private slots:
  void openDatabase();
  void tableSelected(int index);

private:
  void setupUI();
  void loadTables();
  QString database_path;
  QSqlDatabase m_database;
  QSqlTableModel *m_tableModel;
  QSqlQueryModel *m_queryModel;
  bool m_useQueryModel;

  QComboBox *m_tableComboBox;
  QTableView *m_tableView;
  QPushButton *m_openButton;
};

#endif // VISOR_H
