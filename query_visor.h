#ifndef QUERY_VISOR_H
#define QUERY_VISOR_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QComboBox>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>

class InputDialog : public QDialog
{
  Q_OBJECT
public:
  explicit InputDialog(const QString &prompt, QWidget *parent = nullptr);
  QString getValue() const;

private:
  QLineEdit *m_input;
};

class QueryVisor : public QWidget
{
  Q_OBJECT

public:
  explicit QueryVisor(QString database_path_,QWidget *parent = nullptr);
  ~QueryVisor();

private slots:
  void openDatabase();
  void executeQuery(int index);
  void showInputDialog();

private:
  QString database_path;
  void setupUI();
  void setupQueries();
  void runQuery(const QString &query); // Добавлен этот метод

  QSqlDatabase m_database;
  QSqlQueryModel *m_model;

  QComboBox *m_queryComboBox;
  QTableView *m_tableView;
  QPushButton *m_openButton;

  QString m_currentQuery;
};

#endif // QUERY_VISOR_H
