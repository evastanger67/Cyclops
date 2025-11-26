#include "main_visor.h"
#include "visor.h"
#include "query_visor.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QFileInfo>

MainVisor::MainVisor(QWidget *parent)
    : QWidget(parent)
{
  setupUI();
  setWindowTitle("SQLite Tools");
  resize(400, 200);
}

void MainVisor::setupUI()
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(15);

         // Кнопка открытия базы данных
  m_openDbBtn = new QPushButton("Открыть базу данных", this);
  m_openDbBtn->setMinimumHeight(40);

  // Статус базы данных
  m_statusLabel = new QLabel("База данных не открыта", this);
  m_statusLabel->setAlignment(Qt::AlignCenter);
  m_statusLabel->setStyleSheet("QLabel { background-color: #000000; padding: 8px; border: 1px solid #cc0000; }");

  // Кнопки инструментов
  QHBoxLayout *toolsLayout = new QHBoxLayout();
  m_tableViewerBtn = new QPushButton("Просмотрщик таблиц", this);
  m_queryExecutorBtn = new QPushButton("Исполнитель SQL", this);

  m_tableViewerBtn->setMinimumHeight(40);
  m_queryExecutorBtn->setMinimumHeight(40);

  m_tableViewerBtn->setEnabled(false);
  m_queryExecutorBtn->setEnabled(false);

  toolsLayout->addWidget(m_tableViewerBtn);
  toolsLayout->addWidget(m_queryExecutorBtn);

  layout->addWidget(m_openDbBtn);
  layout->addWidget(m_statusLabel);
  layout->addLayout(toolsLayout);
  layout->addStretch();

  connect(m_openDbBtn, &QPushButton::clicked, this, &MainVisor::openDatabase);
  connect(m_tableViewerBtn, &QPushButton::clicked, this, &MainVisor::openTableViewer);
  connect(m_queryExecutorBtn, &QPushButton::clicked, this, &MainVisor::openQueryExecutor);
}

void MainVisor::openDatabase()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                   "Открыть базу данных SQLite",
                                                   QDir::homePath(),
                                                   "SQLite Databases (*.db *.sqlite *.sqlite3);;Все файлы (*)");

  if (fileName.isEmpty()) {
      return;
    }
  QSqlDatabase testDb = QSqlDatabase::addDatabase("QSQLITE", "test_connection");
  testDb.setDatabaseName(fileName);

  if (!testDb.open()) {
      QMessageBox::critical(this, "Ошибка",
                             QString("Не удалось открыть базу данных:\n%1\n%2")
                                 .arg(fileName)
                                 .arg(testDb.lastError().text()));
      testDb = QSqlDatabase();
      QSqlDatabase::removeDatabase("test_connection");
      return;
    }

  testDb.close();
  testDb = QSqlDatabase();
  QSqlDatabase::removeDatabase("test_connection");

  m_databasePath = fileName;
  updateDatabaseStatus();
}

void MainVisor::updateDatabaseStatus()
{
  if (m_databasePath.isEmpty()) {
      m_statusLabel->setText("База данных не открыта");
      m_statusLabel->setStyleSheet("QLabel { background-color: #ffcccc; padding: 8px; border: 1px solid #cc0000; }");
      m_tableViewerBtn->setEnabled(false);
      m_queryExecutorBtn->setEnabled(false);
    } else {
      QString dbName = QFileInfo(m_databasePath).fileName();
      m_statusLabel->setText(QString("База данных: %1").arg(dbName));
      m_statusLabel->setStyleSheet("QLabel { background-color: gray; padding: 8px; border: 1px solid #00cc00; }");
      m_tableViewerBtn->setEnabled(true);
      m_queryExecutorBtn->setEnabled(true);
    }
}

void MainVisor::openTableViewer()
{
  if (m_databasePath.isEmpty()) {
      QMessageBox::warning(this, "Ошибка", "Сначала откройте базу данных");
      return;
    }

  Visor *viewer = new Visor(m_databasePath);
  viewer->setAttribute(Qt::WA_DeleteOnClose);
  viewer->resize(1200, 700);
  viewer->show();
}

void MainVisor::openQueryExecutor()
{
  if (m_databasePath.isEmpty()) {
      QMessageBox::warning(this, "Ошибка", "Сначала откройте базу данных");
      return;
    }

  QueryVisor *executor = new QueryVisor(m_databasePath);
  executor->setAttribute(Qt::WA_DeleteOnClose);
  executor->resize(900, 500);
  executor->show();
}
