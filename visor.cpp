#include "visor.h"
Visor::Visor(QString database_path_,QWidget *parent)
    : QWidget(parent)
      , m_tableModel(nullptr)
      , m_queryModel(nullptr)
      , m_useQueryModel(false)
{
  database_path = database_path_;
  setupUI();

  m_tableModel = new QSqlTableModel(this);
  m_queryModel = new QSqlQueryModel(this);

  m_tableView->setModel(m_tableModel);
  m_tableView->setAlternatingRowColors(true);
  setWindowTitle("Visor");
  resize(800, 600);
}

Visor::~Visor()
{
  if (m_database.isOpen()) {
      m_database.close();
    }
}
void Visor::setupUI()
{
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 5, 5, 5);
  QHBoxLayout *topLayout = new QHBoxLayout();
  m_openButton = new QPushButton("Open", this);
  m_tableComboBox = new QComboBox(this);
  m_tableComboBox->setEnabled(false);
  m_tableComboBox->setFixedWidth(500);
  topLayout->addWidget(m_openButton);
  topLayout->addWidget(m_tableComboBox);
  topLayout->addStretch();
  m_tableView = new QTableView(this);
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(m_tableView);
  connect(m_openButton, &QPushButton::clicked, this, &Visor::openDatabase);
  connect(m_tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
           this, &Visor::tableSelected);
}
void Visor::openDatabase()
{
  QString fileName = database_path;

  if (fileName.isEmpty()) {
      return;
    }

  if (m_database.isOpen()) {
      m_database.close();
    }

  m_database = QSqlDatabase::addDatabase("QSQLITE");
  m_database.setDatabaseName(fileName);

  if (!m_database.open()) {
      QMessageBox::critical(this, "ERR", "database loading");
      return;
    }

  setWindowTitle(QString("SQLite Visor - %1").arg(QFileInfo(fileName).fileName()));
  loadTables();
}

void Visor::loadTables()
{
  if (!m_database.isOpen()) {
      return;
    }

  m_tableComboBox->clear();
  m_tableComboBox->setEnabled(true);

  QStringList tables = m_database.tables(QSql::Tables);
  for (const QString &table : tables) {
      m_tableComboBox->addItem(table);
    }

  if (!tables.isEmpty()) {
      m_tableComboBox->setCurrentIndex(0);
    }
}

void Visor::tableSelected(int index)
{
  if (!m_database.isOpen() || index < 0) {
      return;
    }

  QString tableName = m_tableComboBox->itemText(index);

  if (m_useQueryModel) {
      m_queryModel->setQuery(QString("SELECT * FROM `%1`").arg(tableName), m_database);
      if (!m_queryModel->lastError().isValid()) {
          m_tableView->setModel(m_queryModel);
        }
    } else {
      m_tableModel->setTable(tableName);
      if (m_tableModel->select()) {
          m_tableView->setModel(m_tableModel);
        } else {
          m_useQueryModel = true;
          m_queryModel->setQuery(QString("SELECT * FROM `%1`").arg(tableName), m_database);
          if (!m_queryModel->lastError().isValid()) {
              m_tableView->setModel(m_queryModel);
            }
        }
    }

  m_tableView->resizeColumnsToContents();
}
