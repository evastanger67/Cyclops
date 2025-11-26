#include "query_visor.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>

InputDialog::InputDialog(const QString &prompt, QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle("Ввод параметра");
  setModal(true);

  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->addWidget(new QLabel(prompt, this));

  m_input = new QLineEdit(this);
  layout->addWidget(m_input);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout->addWidget(buttons);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString InputDialog::getValue() const
{
  return m_input->text();
}

QueryVisor::QueryVisor(QString database_path_,QWidget *parent)
    : QWidget(parent)
      , m_model(nullptr)
{
  database_path = database_path_;
  setupUI();
  setupQueries();

  m_model = new QSqlQueryModel(this);
  m_tableView->setModel(m_model);
  m_tableView->setAlternatingRowColors(true);

  setWindowTitle("SQL Query Visor");
  resize(800, 600);
}

QueryVisor::~QueryVisor()
{
  if (m_database.isOpen()) {
      m_database.close();
    }
}

void QueryVisor::setupUI()
{
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 5, 5, 5);

  QHBoxLayout *topLayout = new QHBoxLayout();

  m_openButton = new QPushButton("Открыть БД", this);
  m_queryComboBox = new QComboBox(this);

  topLayout->addWidget(m_openButton);
  topLayout->addWidget(m_queryComboBox);
  topLayout->addStretch();

  m_tableView = new QTableView(this);

  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(m_tableView);

  connect(m_openButton, &QPushButton::clicked, this, &QueryVisor::openDatabase);
  connect(m_queryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
           this, &QueryVisor::executeQuery);
}

void QueryVisor::setupQueries()
{
  m_queryComboBox->addItem("Выберите запрос...");
  m_queryComboBox->addItem("2-1",
                            "SELECT НАЛОГИ.НАИМНАЛ, ПРЕДПРИЯТИЯ.НАИМПРЕД, ПЛАТЕЖИ.НОМЕРДОК, СТРОКИ.СУММА FROM НАЛОГИ, ПРЕДПРИЯТИЯ, ПЛАТЕЖИ, СТРОКИ WHERE ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН AND ПЛАТЕЖИ.НОМЕРДОК = СТРОКИ.НОМЕРДОК AND СТРОКИ.КОДНАЛ = НАЛОГИ.КОДНАЛ;");
  m_queryComboBox->addItem("2-2", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД, ПЛАТЕЖИ.НОМЕРДОК, ПЛАТЕЖИ.ДАТА FROM ПРЕДПРИЯТИЯ, ПЛАТЕЖИ WHERE ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН;");
  m_queryComboBox->addItem("2-3", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД FROM ПРЕДПРИЯТИЯ JOIN ПЛАН ON ПРЕДПРИЯТИЯ.ИНН = ПЛАН.ИНН AND ПЛАН.КОДНАЛ = \"01\";");
  m_queryComboBox->addItem("2-4", "SELECT ПЛАТЕЖИ.НОМЕРДОК, ПЛАТЕЖИ.ДАТА FROM ПЛАТЕЖИ WHERE ПЛАТЕЖИ.ИНН = \"783333333333\"");
  m_queryComboBox->addItem("2-5", "SELECT НАЛОГИ.НАИМНАЛ, СТРОКИ.СУММА FROM НАЛОГИ, СТРОКИ JOIN ПЛАТЕЖИ ON ПЛАТЕЖИ.ДАТА = \"2023-01-31\" AND СТРОКИ.НОМЕРДОК = ПЛАТЕЖИ.НОМЕРДОК AND НАЛОГИ.КОДНАЛ = СТРОКИ.КОДНАЛ;");
  m_queryComboBox->addItem("2-6", "SELECT DISTINCT ПРЕДПРИЯТИЯ.НАИМПРЕД FROM ПРЕДПРИЯТИЯ JOIN ПЛАТЕЖИ ON ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН JOIN СТРОКИ  ON СТРОКИ.НОМЕРДОК = ПЛАТЕЖИ.НОМЕРДОК AND СТРОКИ.КОДНАЛ = '{codnal}'");
  m_queryComboBox->addItem("2-7", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД, ПЛАТЕЖИ.НОМЕРДОК FROM ПРЕДПРИЯТИЯ, ПЛАТЕЖИ WHERE ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН AND ПЛАТЕЖИ.ДАТА = \"{date}\" ");
  m_queryComboBox->addItem("2-8", "SELECT ПЛАН.НАЛБАЗА, НАЛОГИ.НАИМНАЛ FROM ПЛАН, НАЛОГИ JOIN ПРЕДПРИЯТИЯ ON НАЛОГИ.КОДНАЛ = ПЛАН.КОДНАЛ AND ПЛАН.ИНН = ПРЕДПРИЯТИЯ.ИНН AND ПРЕДПРИЯТИЯ.НАИМПРЕД = \"{naimpred}\";");
  m_queryComboBox->addItem("3-1", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД,  НАЛОГИ.НАИМНАЛ,  НАЛОГИ.ПРОЦНАЛ,  ПЛАН.НАЛБАЗА, ROUND((ПЛАН.НАЛБАЗА * НАЛОГИ.ПРОЦНАЛ / 100 / {rate} ),2) || \" USD\" AS СУММА FROM ПРЕДПРИЯТИЯ, НАЛОГИ, ПЛАН WHERE ПРЕДПРИЯТИЯ.ИНН = ПЛАН.ИНН AND ПЛАН.КОДНАЛ = НАЛОГИ.КОДНАЛ");
  m_queryComboBox->addItem("3-2", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД, НАЛОГИ.НАИМНАЛ FROM ПРЕДПРИЯТИЯ, НАЛОГИ JOIN ПЛАТЕЖИ, СТРОКИ ON ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН AND СТРОКИ.НОМЕРДОК = ПЛАТЕЖИ.НОМЕРДОК AND НАЛОГИ.КОДНАЛ = СТРОКИ.КОДНАЛ AND strftime(\"%m\",ПЛАТЕЖИ.ДАТА) = \"{month}\"");
  m_queryComboBox->addItem("3-3", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД,SUM(СТРОКИ.СУММА) FROM ПРЕДПРИЯТИЯ, СТРОКИ JOIN ПЛАТЕЖИ  ON ПЛАТЕЖИ.ИНН = ПРЕДПРИЯТИЯ.ИНН AND СТРОКИ.НОМЕРДОК = ПЛАТЕЖИ.НОМЕРДОК AND strftime(\"%m\",ПЛАТЕЖИ.ДАТА) = \"{month}\" GROUP BY ПРЕДПРИЯТИЯ.НАИМПРЕД");
  m_queryComboBox->addItem("3-4", "SELECT НАЛОГИ.НАИМНАЛ, SUM (СТРОКИ.СУММА) FROM НАЛОГИ, СТРОКИ JOIN ПЛАТЕЖИ  ON СТРОКИ.НОМЕРДОК = ПЛАТЕЖИ.НОМЕРДОК AND  strftime(\"%m\",ПЛАТЕЖИ.ДАТА) = \"{month}\" WHERE НАЛОГИ.КОДНАЛ = СТРОКИ.КОДНАЛ GROUP BY НАЛОГИ.НАИМНАЛ");
  m_queryComboBox->addItem("3-5", "SELECT ПРЕДПРИЯТИЯ.НАИМПРЕД, ПЛАТЕЖИ.НОМЕРДОК, COUNT(СТРОКИ.КОДНАЛ) AS КОЛВОНАЛОГОВ FROM ПРЕДПРИЯТИЯ, ПЛАТЕЖИ, СТРОКИ WHERE ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН AND ПЛАТЕЖИ.НОМЕРДОК = СТРОКИ.НОМЕРДОК GROUP BY ПЛАТЕЖИ.НОМЕРДОК");
  m_queryComboBox->addItem("4-1", "SELECT ПРЕДПРИЯТИЯ.ИНН,ПРЕДПРИЯТИЯ.НАИМПРЕД, НАЛОГИ.НАИМНАЛ, ABS(ПЛАН.НАЛБАЗА * НАЛОГИ.ПРОЦНАЛ / 100 - COALESCE(( SELECT SUM(СТРОКИ.СУММА) FROM СТРОКИ JOIN ПЛАТЕЖИ ON ПЛАТЕЖИ.НОМЕРДОК = СТРОКИ.НОМЕРДОК WHERE ПЛАТЕЖИ.ИНН = ПРЕДПРИЯТИЯ.ИНН AND СТРОКИ.КОДНАЛ = НАЛОГИ.КОДНАЛ), 0)) AS ДОЛГ FROM ПРЕДПРИЯТИЯ JOIN ПЛАН ON ПРЕДПРИЯТИЯ.ИНН = ПЛАН.ИНН JOIN НАЛОГИ ON ПЛАН.КОДНАЛ = НАЛОГИ.КОДНАЛ GROUP BY  НАЛОГИ.НАИМНАЛ, ПРЕДПРИЯТИЯ.ИНН, ПРЕДПРИЯТИЯ.НАИМПРЕД, ПЛАН.НАЛБАЗА, НАЛОГИ.ПРОЦНАЛ ORDER BY ПРЕДПРИЯТИЯ.ИНН;");
  m_queryComboBox->addItem("4-2", "SELECT ПРЕДПРИЯТИЯ.ИНН, ПРЕДПРИЯТИЯ.НАИМПРЕД, НАЛОГИ.КОДНАЛ, НАЛОГИ.НАИМНАЛ, ПЛАН.НАЛБАЗА FROM ПРЕДПРИЯТИЯ JOIN ПЛАН ON ПРЕДПРИЯТИЯ.ИНН = ПЛАН.ИНН JOIN НАЛОГИ ON ПЛАН.КОДНАЛ = НАЛОГИ.КОДНАЛ WHERE NOT EXISTS ( SELECT 1 FROM ПЛАТЕЖИ  JOIN СТРОКИ ON ПЛАТЕЖИ.НОМЕРДОК = СТРОКИ.НОМЕРДОК WHERE ПЛАТЕЖИ.ИНН = ПРЕДПРИЯТИЯ.ИНН AND СТРОКИ.КОДНАЛ = НАЛОГИ.КОДНАЛ);");
  m_queryComboBox->addItem("НЕ РАБОТАЕТ", "CREATE TEMP TABLE IF NOT EXISTS VARS(VALUE INTEGER);INSERT OR REPLACE INTO VARS VALUES (1/*МЕНЯТЬ ТУТ*/); SELECT  НАЛОГИ.НАИМНАЛ, (ПЛАН.НАЛБАЗА * НАЛОГИ.ПРОЦНАЛ / 100 / 4) - COALESCE(SUM(СТРОКИ.СУММА), 0) AS \"НЕДОПЛАТА\" FROM ПРЕДПРИЯТИЯ JOIN ПЛАН ON ПРЕДПРИЯТИЯ.ИНН = ПЛАН.ИНН JOIN НАЛОГИ ON ПЛАН.КОДНАЛ = НАЛОГИ.КОДНАЛ LEFT JOIN ПЛАТЕЖИ ON ПРЕДПРИЯТИЯ.ИНН = ПЛАТЕЖИ.ИНН LEFT JOIN СТРОКИ ON ПЛАТЕЖИ.НОМЕРДОК = СТРОКИ.НОМЕРДОК AND СТРОКИ.КОДНАЛ = НАЛОГИ.КОДНАЛ WHERE ПРЕДПРИЯТИЯ.НАИМПРЕД = 'АЛЕКСЕЕВ'  AND CASE  WHEN (SELECT VALUE FROM VARS) = 1 THEN strftime('%m', ПЛАТЕЖИ.ДАТА) BETWEEN '01' AND '03' WHEN (SELECT VALUE FROM VARS) = 2 THEN strftime('%m', ПЛАТЕЖИ.ДАТА) BETWEEN '04' AND '06' WHEN (SELECT VALUE FROM VARS) = 3 THEN strftime('%m', ПЛАТЕЖИ.ДАТА) BETWEEN '07' AND '09' WHEN (SELECT VALUE FROM VARS) = 4 THEN strftime('%m', ПЛАТЕЖИ.ДАТА) BETWEEN '10' AND '12' END  GROUP BY НАЛОГИ.НАИМНАЛ HAVING \"НЕДОПЛАТА\" > 0;");
}

void QueryVisor::openDatabase()
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
      QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
      return;
    }

  setWindowTitle(QString("Query Visor - %1").arg(QFileInfo(fileName).fileName()));
  m_queryComboBox->setCurrentIndex(0);
}

void QueryVisor::executeQuery(int index)
{
  if (index <= 0 || !m_database.isOpen()) {
      return;
    }

  m_currentQuery = m_queryComboBox->currentData().toString();

  if (m_currentQuery.contains('{')) {
      showInputDialog();
    } else {
      runQuery(m_currentQuery);
    }
}

void QueryVisor::showInputDialog()
{
  QString prompt;
  QString paramName;

  if (m_currentQuery.contains("{codnal}")) {
      prompt = "Введите код налога в формате 0X:";
      paramName = "codnal";
    } else if (m_currentQuery.contains("{date}")) {
      prompt = "Введите дату:";
      paramName = "date";
    } else if (m_currentQuery.contains("{naimpred}")) {
      prompt = "Введите наименование предприятия";
      paramName = "naimpred";
    } else if (m_currentQuery.contains("{rate}")) {
      prompt = "Введите курс доллара";
      paramName = "rate";
    } else if (m_currentQuery.contains("{month}")) {
      prompt = "Введите месяц";
      paramName = "month";
    }

  InputDialog dialog(prompt, this);
  if (dialog.exec() == QDialog::Accepted) {
      QString value = dialog.getValue();
      if (!value.isEmpty()) {
          QString finalQuery = m_currentQuery;
          finalQuery.replace("{" + paramName + "}", value);
          runQuery(finalQuery);
        }
    } else {
      m_queryComboBox->setCurrentIndex(0);
    }
}

void QueryVisor::runQuery(const QString &query)
{
  m_model->setQuery(query, m_database);

  if (m_model->lastError().isValid()) {
      QMessageBox::warning(this, "Query error", m_model->lastError().text());
      m_queryComboBox->setCurrentIndex(0);
    } else {
      m_tableView->resizeColumnsToContents();
    }
}
