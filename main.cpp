// Transaction.h


#include <QDateTime>
#include <QString>

enum class TransactionType { Deposit, Withdrawal };

class Transaction {
public:
    Transaction(const QDateTime& timestamp, double amount, TransactionType type);
    ~Transaction() = default;

    QDateTime timestamp() const;
    double amount() const;
    TransactionType type() const;

    QString toString() const;

private:
    QDateTime m_timestamp;
    double m_amount;
    TransactionType m_type;
};


// Transaction.cpp
#include "Transaction.h"

Transaction::Transaction(const QDateTime& timestamp, double amount, TransactionType type)
    : m_timestamp(timestamp), m_amount(amount), m_type(type) {}

QDateTime Transaction::timestamp() const { return m_timestamp; }
double Transaction::amount() const { return m_amount; }
TransactionType Transaction::type() const { return m_type; }

QString Transaction::toString() const {
    QString typeStr = (m_type == TransactionType::Deposit ? "Deposit" : "Withdrawal");
    return QString("%1: R%2 on %3")
        .arg(typeStr)
        .arg(m_amount, 0, 'f', 2)
        .arg(m_timestamp.toString("ddd MMM dd yyyy 'at' hh:mm:ss"));
}


// TransactionList.h


#include <vector>
#include <QString>

class Transaction;

class TransactionList {
public:
    static TransactionList& instance();
    void addTransaction(Transaction* t);
    const std::vector<Transaction*>& transactions() const;
    bool writeToFile(const QString& fileName) const;

private:
    TransactionList() = default;
    ~TransactionList();
    TransactionList(const TransactionList&) = delete;
    TransactionList& operator=(const TransactionList&) = delete;

    std::vector<Transaction*> m_list;
};


// TransactionList.cpp
#include "TransactionList.h"
#include "Transaction.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

TransactionList& TransactionList::instance() {
    static TransactionList inst;
    return inst;
}

void TransactionList::addTransaction(Transaction* t) {
    m_list.push_back(t);
    qDebug() << t->toString();
}

const std::vector<Transaction*>& TransactionList::transactions() const {
    return m_list;
}

bool TransactionList::writeToFile(const QString& fileName) const {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return false;
    }
    QTextStream out(&file);
    for (auto t : m_list) {
        out << t->toString() << "\n";
    }
    file.close();
    return true;
}

TransactionList::~TransactionList() {
    for (auto t : m_list)
        delete t;
    m_list.clear();
}


// main.cpp
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QDate>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>

#include "Transaction.h"
#include "TransactionList.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Banking transactions");

    // Date display
    auto *dateLabel = new QLabel("Date:", &window);
    auto *dateValue = new QLabel(QDate::currentDate().toString("ddd MMM dd yyyy"), &window);

    // Amount input
    auto *amountLabel = new QLabel("Amount:", &window);
    auto *amountSpin = new QDoubleSpinBox(&window);
    amountSpin->setPrefix("R");
    amountSpin->setRange(0, 1e9);
    amountSpin->setDecimals(2);

    // Buttons
    auto *depositBtn = new QPushButton("Deposit", &window);
    auto *withdrawBtn = new QPushButton("Withdrawal", &window);
    auto *toFileBtn = new QPushButton("To file", &window);

    // Layout
    auto *layout = new QGridLayout(&window);
    layout->addWidget(dateLabel, 0, 0);
    layout->addWidget(dateValue, 0, 1);
    layout->addWidget(amountLabel, 1, 0);
    layout->addWidget(amountSpin, 1, 1);
    layout->addWidget(depositBtn, 2, 0);
    layout->addWidget(withdrawBtn, 2, 1);
    layout->addWidget(toFileBtn, 3, 0, 1, 2);

    // Signals → create transactions
    QObject::connect(depositBtn, &QPushButton::clicked, [&]() {
        double amt = amountSpin->value();
        auto *t = new Transaction(QDateTime::currentDateTime(), amt, TransactionType::Deposit);
        TransactionList::instance().addTransaction(t);
    });
    QObject::connect(withdrawBtn, &QPushButton::clicked, [&]() {
        double amt = amountSpin->value();
        auto *t = new Transaction(QDateTime::currentDateTime(), amt, TransactionType::Withdrawal);
        TransactionList::instance().addTransaction(t);
    });
    QObject::connect(toFileBtn, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&window, "Save transactions", "transactions.txt", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            TransactionList::instance().writeToFile(fileName);
        }
    });

    window.setLayout(layout);
    window.show();
    return app.exec();
}
