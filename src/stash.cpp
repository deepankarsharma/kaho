class CompletionModel : public QStringListModel
{
  Q_OBJECT

 public:
  explicit CompletionModel(QObject *parent = nullptr)
      : QStringListModel(parent), m_aimodel(new AIModel(this))
  {
    m_networkManager = new QNetworkAccessManager(this);
    QObject::connect(
        m_aimodel, &AIModel::answerFragmentReceived,
        this,
        [this](const QString& response) {
          auto first_brace_index = response.indexOf('{');
          if (first_brace_index >= 1) {
            auto rest = response.mid(first_brace_index - 1);
            QJsonDocument jsonDoc = QJsonDocument::fromJson(rest.toUtf8());
            auto content = jsonDoc["choices"][0]["delta"]["content"];
            m_completion += content.toString();
            if (m_completions.empty() && insertRow(0)) {
              QModelIndex index = this->index(0, 0);
              setData(index, m_completion);
            }

            qDebug() << "===> Completion request: " << m_completion;
          }
        });
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
  {
    if (!hasIndex(row, column, parent))
      return QModelIndex();

    return createIndex(row, column);
  }

  QModelIndex parent(const QModelIndex &child) const override
  {
    Q_UNUSED(child);
    return QModelIndex();
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override
  {
    if (parent.isValid())
      return 0;

    return m_completions.count();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);
    return 1;
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
  {
    qDebug() << "CompletionModel::data index => " << index << " role ==> " << role << " options ==> " << m_completions;

    if (!index.isValid())
      return QVariant();

    if (role == Qt::DisplayRole)
      return m_completions[index.row()];

    return QVariant();
  }

 public slots:
  void prefixChanged(const QString& prefix) { m_completion = ""; emit m_aimodel->processPrompt(prefix); }

 signals:
  void completionListChanged();

 private:
  AIModel* m_aimodel;
  QNetworkAccessManager *m_networkManager;
  QStringList m_completions;
  QString m_completion;
};

