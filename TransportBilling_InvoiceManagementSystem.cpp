#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>

using namespace std;

class Client {
private:
    int clientId;
    string name;
    string contactNumber;

public:
    Client(int id, string n, string contact) {
        clientId = id;
        name = n;
        contactNumber = contact;
    }

    int getId() const { return clientId; }
    string getName() const { return name; }
    string getContact() const { return contactNumber; }
};

class Trip {
private:
    int tripId;
    double distanceKm;
    double ratePerKm;
    double baseFare;
    double extraCharges;

public:
    Trip(int id, double dist, double rate, double base, double extra) {
        tripId = id;
        distanceKm = dist;
        ratePerKm = rate;
        baseFare = base;
        extraCharges = extra;
    }

    double calculateTotalCost() const {
        double subtotal = baseFare + (distanceKm * ratePerKm) + extraCharges;
        double gst = subtotal * 0.18;
        return subtotal + gst;
    }

    double getSubtotal() const { return baseFare + (distanceKm * ratePerKm) + extraCharges; }
    double getGST() const { return getSubtotal() * 0.18; }

    int getTripId() const { return tripId; }
    double getDistance() const { return distanceKm; }
    double getBaseFare() const { return baseFare; }
    double getExtra() const { return extraCharges; }
    double getRate() const { return ratePerKm; }
};

class Invoice {
private:
    int invoiceId;
    Client* client;
    Trip* trip;
    double totalAmount;
    bool isPaid;

public:
    Invoice(int id, Client* c, Trip* t) {
        invoiceId = id;
        client = c;
        trip = t;
        totalAmount = trip->calculateTotalCost();
        isPaid = false;
    }

    void generateInvoice() const {
        cout << "\n==========================================\n";
        cout << "               INVOICE #" << invoiceId << "\n";
        cout << "==========================================\n";
        cout << "Client Details:\n";
        cout << "Name:    " << client->getName() << "\n";
        cout << "Contact: " << client->getContact() << "\n";
        cout << "------------------------------------------\n";
        cout << "Trip Details (Trip ID: " << trip->getTripId() << "):\n";
        cout << fixed << setprecision(2);
        cout << "Distance:       " << trip->getDistance() << " km\n";
        cout << "Rate per km:   Rs." << trip->getRate() << "\n";
        cout << "Base Fare:     Rs." << trip->getBaseFare() << "\n";
        cout << "Extra Charges: Rs." << trip->getExtra() << "\n";
        cout << "------------------------------------------\n";
        cout << "Subtotal:      Rs." << trip->getSubtotal() << "\n";
        cout << "GST (18%):     Rs." << trip->getGST() << "\n";
        cout << "------------------------------------------\n";
        cout << "TOTAL AMOUNT:  Rs." << totalAmount << "\n";
        cout << "STATUS:        " << (isPaid ? "PAID" : "PENDING") << "\n";
        cout << "==========================================\n";
    }

    void saveToFile() const {
        string filename = "Invoice_" + to_string(invoiceId) + ".txt";
        ofstream outFile(filename);

        if (outFile.is_open()) {
            outFile << "\n==========================================\n";
            outFile << "               INVOICE #" << invoiceId << "\n";
            outFile << "==========================================\n";
            outFile << "Client Details:\n";
            outFile << "Name:    " << client->getName() << "\n";
            outFile << "Contact: " << client->getContact() << "\n";
            outFile << "------------------------------------------\n";
            outFile << "Trip Details (Trip ID: " << trip->getTripId() << "):\n";
            outFile << fixed << setprecision(2);
            outFile << "Distance:       " << trip->getDistance() << " km\n";
            outFile << "Rate per km:   Rs." << trip->getRate() << "\n";
            outFile << "Base Fare:     Rs." << trip->getBaseFare() << "\n";
            outFile << "Extra Charges: Rs." << trip->getExtra() << "\n";
            outFile << "------------------------------------------\n";
            outFile << "Subtotal:      Rs." << trip->getSubtotal() << "\n";
            outFile << "GST (18%):     Rs." << trip->getGST() << "\n";
            outFile << "------------------------------------------\n";
            outFile << "TOTAL AMOUNT:  Rs." << totalAmount << "\n";
            outFile << "STATUS:        " << (isPaid ? "PAID" : "PENDING") << "\n";
            outFile << "==========================================\n";

            outFile.close();
            cout << "[INFO] Invoice successfully saved to " << filename << "\n";
        } else {
            cout << "[ERROR] Unable to create invoice file.\n";
        }
    }

    void markAsPaid() { isPaid = true; }

    bool getPaymentStatus() const { return isPaid; }
    int getInvoiceId() const { return invoiceId; }
    double getTotal() const { return totalAmount; }
};

class Payment {
private:
    int paymentId;
    Invoice* invoice;
    double amountPaid;

public:
    Payment(int id, Invoice* inv, double amount) {
        paymentId = id;
        invoice = inv;
        amountPaid = amount;
    }

    void processPayment() {
        if (amountPaid >= invoice->getTotal()) {
            invoice->markAsPaid();
            cout << "\n[SUCCESS] Payment of Rs." << amountPaid << " processed successfully.\n";
            if (amountPaid > invoice->getTotal()) {
                cout << "[INFO] Change to return: Rs." << (amountPaid - invoice->getTotal()) << "\n";
            }
        } else {
            cout << "\n[ERROR] Insufficient payment. Rs." << invoice->getTotal() << " is required.\n";
        }
    }
};

double getNumber(string prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "[ERROR] Invalid input. Please enter a number.\n";
        cout << prompt;
    }
    return value;
}

int main() {
    vector<Client*> clients;
    vector<Trip*> trips;
    vector<Invoice*> invoices;
    vector<Payment*> payments;

    int choice;
    int clientCounter  = 101;
    int tripCounter    = 501;
    int invoiceCounter = 1001;
    int paymentCounter = 9001;

    cout << "***************************************************\n";
    cout << "* ITM Skill University - B.Tech CSE System        *\n";
    cout << "* Transport Billing & Invoice Management System   *\n";
    cout << "***************************************************\n";

    do {
        cout << "\n--- MAIN MENU ---\n";
        cout << "1. Register New Client\n";
        cout << "2. Log New Transport Trip\n";
        cout << "3. Generate Invoice\n";
        cout << "4. Process Payment\n";
        cout << "5. View All Invoices\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            string name, contact;
            cout << "Enter Client Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter Contact Number: ";
            getline(cin, contact);

            clients.push_back(new Client(clientCounter, name, contact));
            cout << "[SUCCESS] Client registered with ID: " << clientCounter << "\n";
            clientCounter++;

        } else if (choice == 2) {
            if (clients.empty()) {
                cout << "[ERROR] Please register a client first!\n";
                continue;
            }

            double dist  = getNumber("Enter Distance (km): ");
            double rate  = getNumber("Enter Rate per km (Rs): ");
            double base  = getNumber("Enter Base Fare (Rs): ");
            double extra = getNumber("Enter Extra Charges/Tolls (Rs): ");

            trips.push_back(new Trip(tripCounter, dist, rate, base, extra));
            cout << "[SUCCESS] Trip logged with ID: " << tripCounter << "\n";
            tripCounter++;

        } else if (choice == 3) {
            if (clients.empty() || trips.empty()) {
                cout << "[ERROR] Ensure at least one client and one trip exist.\n";
                continue;
            }

            Client* currentClient = clients.back();
            Trip* currentTrip = trips.back();

            Invoice* newInvoice = new Invoice(invoiceCounter, currentClient, currentTrip);
            invoices.push_back(newInvoice);

            newInvoice->generateInvoice();
            newInvoice->saveToFile();

            invoiceCounter++;

        } else if (choice == 4) {
            if (invoices.empty()) {
                cout << "[ERROR] No invoices generated yet!\n";
                continue;
            }

            Invoice* pendingInvoice = invoices.back();

            if (pendingInvoice->getPaymentStatus()) {
                cout << "[INFO] The latest invoice is already paid.\n";
                continue;
            }

            cout << fixed << setprecision(2);
            cout << "Total Due: Rs." << pendingInvoice->getTotal() << "\n";

            double paymentAmt = getNumber("Enter payment amount: Rs.");

            Payment* pay = new Payment(paymentCounter, pendingInvoice, paymentAmt);
            pay->processPayment();
            payments.push_back(pay);
            paymentCounter++;

            if (pendingInvoice->getPaymentStatus()) {
                pendingInvoice->saveToFile();
            }

        } else if (choice == 5) {
            if (invoices.empty()) {
                cout << "[INFO] No invoices yet.\n";
                continue;
            }
            cout << "\n--- ALL INVOICES ---\n";
            for (Invoice* inv : invoices) {
                inv->generateInvoice();
            }

        } else if (choice != 6) {
            cout << "[ERROR] Invalid choice. Try again.\n";
        }

    } while (choice != 6);

    cout << "Exiting system. Goodbye!\n";

    for (Client* c  : clients)  delete c;
    for (Trip* t    : trips)    delete t;
    for (Invoice* i : invoices) delete i;
    for (Payment* p : payments) delete p;

    return 0;
}