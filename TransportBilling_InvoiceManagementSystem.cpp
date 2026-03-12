#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Client {
private:
  int clientId;
  string name;
  string contactNumber;

public:
  Client(int id, string n, string contact)
      : clientId(id), name(move(n)), contactNumber(move(contact)) {}

  int getId() const { return clientId; }
  string getName() const { return name; }
  string getContact() const { return contactNumber; }

  void display() const {
    cout << "  [ID: " << clientId << "]  " << name << "  |  " << contactNumber << "\n";
  }

  string toCSV() const {
    return to_string(clientId) + "," + name + "," + contactNumber;
  }

  static unique_ptr<Client> fromCSV(const string &line) {
    stringstream ss(line);
    string id, nm, contact;
    getline(ss, id, ',');
    getline(ss, nm, ',');
    getline(ss, contact, ',');
    return make_unique<Client>(stoi(id), nm, contact);
  }
};

class Trip {
private:
  int tripId;
  double distanceKm;
  double ratePerKm;
  double baseFare;
  double extraCharges;

public:
  Trip(int id, double dist, double rate, double base, double extra)
      : tripId(id), distanceKm(dist), ratePerKm(rate), baseFare(base),
        extraCharges(extra) {}

  double getSubtotal() const { return baseFare + (distanceKm * ratePerKm) + extraCharges; }
  double getGST()      const { return getSubtotal() * 0.18; }
  double getTotal()    const { return getSubtotal() + getGST(); }

  int    getTripId()   const { return tripId; }
  double getDistance() const { return distanceKm; }
  double getRate()     const { return ratePerKm; }
  double getBaseFare() const { return baseFare; }
  double getExtra()    const { return extraCharges; }

  void display() const {
    cout << fixed << setprecision(2);
    cout << "  [ID: " << tripId << "]  " << distanceKm << " km  @  Rs."
         << ratePerKm << "/km"
         << "  |  Base: Rs." << baseFare << "  |  Extra: Rs." << extraCharges
         << "  =>  Total: Rs." << getTotal() << "\n";
  }

  string toCSV() const {
    return to_string(tripId) + "," + to_string(distanceKm) + "," +
           to_string(ratePerKm) + "," + to_string(baseFare) + "," +
           to_string(extraCharges);
  }

  static unique_ptr<Trip> fromCSV(const string &line) {
    stringstream ss(line);
    string id, dist, rate, base, extra;
    getline(ss, id, ',');
    getline(ss, dist, ',');
    getline(ss, rate, ',');
    getline(ss, base, ',');
    getline(ss, extra, ',');
    return make_unique<Trip>(stoi(id), stod(dist), stod(rate), stod(base), stod(extra));
  }
};

class Invoice {
private:
  int invoiceId;
  Client *client;
  Trip *trip;
  double totalAmount;
  bool isPaid;

  void printTo(ostream &out) const {
    out << "\n==========================================\n";
    out << "               INVOICE #" << invoiceId << "\n";
    out << "==========================================\n";
    out << "Client Details:\n";
    out << "Name:    " << client->getName() << "\n";
    out << "Contact: " << client->getContact() << "\n";
    out << "------------------------------------------\n";
    out << "Trip Details (Trip ID: " << trip->getTripId() << "):\n";
    out << fixed << setprecision(2);
    out << "Distance:       " << trip->getDistance() << " km\n";
    out << "Rate per km:   Rs." << trip->getRate() << "\n";
    out << "Base Fare:     Rs." << trip->getBaseFare() << "\n";
    out << "Extra Charges: Rs." << trip->getExtra() << "\n";
    out << "------------------------------------------\n";
    out << "Subtotal:      Rs." << trip->getSubtotal() << "\n";
    out << "GST (18%):     Rs." << trip->getGST() << "\n";
    out << "------------------------------------------\n";
    out << "TOTAL AMOUNT:  Rs." << totalAmount << "\n";
    out << "STATUS:        " << (isPaid ? "PAID" : "PENDING") << "\n";
    out << "==========================================\n";
  }

public:
  Invoice(int id, Client *c, Trip *t)
      : invoiceId(id), client(c), trip(t), totalAmount(t->getTotal()), isPaid(false) {}

  Invoice(int id, Client *c, Trip *t, double total, bool paid)
      : invoiceId(id), client(c), trip(t), totalAmount(total), isPaid(paid) {}

  void generateInvoice() const { printTo(cout); }

  void saveToFile() const {
    string filename = "Invoice_" + to_string(invoiceId) + ".txt";
    ofstream f(filename);
    if (f.is_open()) {
      printTo(f);
      cout << "[INFO] Invoice saved to " << filename << "\n";
    } else {
      cout << "[ERROR] Unable to create invoice file.\n";
    }
  }

  void markAsPaid() { isPaid = true; }

  bool   getPaymentStatus() const { return isPaid; }
  int    getInvoiceId()     const { return invoiceId; }
  double getTotal()         const { return totalAmount; }
  int    getClientId()      const { return client->getId(); }
  int    getTripId()        const { return trip->getTripId(); }

  string toCSV() const {
    return to_string(invoiceId) + "," + to_string(client->getId()) + "," +
           to_string(trip->getTripId()) + "," + to_string(totalAmount) + "," +
           (isPaid ? "1" : "0");
  }
};

class Payment {
private:
  Invoice *invoice;
  double amountPaid;

public:
  Payment(Invoice *inv, double amount) : invoice(inv), amountPaid(amount) {}

  void processPayment() {
    if (amountPaid >= invoice->getTotal()) {
      invoice->markAsPaid();
      cout << "\n[SUCCESS] Payment of Rs." << fixed << setprecision(2)
           << amountPaid << " processed successfully.\n";
      double change = amountPaid - invoice->getTotal();
      if (change > 0.001)
        cout << "[INFO] Change to return: Rs." << change << "\n";
    } else {
      cout << "\n[ERROR] Insufficient payment. Rs." << fixed << setprecision(2)
           << invoice->getTotal() << " is required.\n";
    }
  }
};

void saveClients(const vector<unique_ptr<Client>> &v) {
  ofstream f("db_clients.csv");
  for (const auto &c : v) f << c->toCSV() << "\n";
}

void saveTrips(const vector<unique_ptr<Trip>> &v) {
  ofstream f("db_trips.csv");
  for (const auto &t : v) f << t->toCSV() << "\n";
}

void saveInvoices(const vector<unique_ptr<Invoice>> &v) {
  ofstream f("db_invoices.csv");
  for (const auto &i : v) f << i->toCSV() << "\n";
}

void loadClients(vector<unique_ptr<Client>> &clients, int &counter) {
  ifstream f("db_clients.csv");
  if (!f.is_open()) return;
  string line;
  while (getline(f, line)) {
    if (line.empty()) continue;
    auto c = Client::fromCSV(line);
    if (c->getId() >= counter) counter = c->getId() + 1;
    clients.push_back(move(c));
  }
  if (!clients.empty())
    cout << "[DB] Loaded " << clients.size() << " client(s).\n";
}

void loadTrips(vector<unique_ptr<Trip>> &trips, int &counter) {
  ifstream f("db_trips.csv");
  if (!f.is_open()) return;
  string line;
  while (getline(f, line)) {
    if (line.empty()) continue;
    auto t = Trip::fromCSV(line);
    if (t->getTripId() >= counter) counter = t->getTripId() + 1;
    trips.push_back(move(t));
  }
  if (!trips.empty())
    cout << "[DB] Loaded " << trips.size() << " trip(s).\n";
}

void loadInvoices(vector<unique_ptr<Invoice>> &invoices,
                  const vector<unique_ptr<Client>> &clients,
                  const vector<unique_ptr<Trip>> &trips, int &counter) {
  ifstream f("db_invoices.csv");
  if (!f.is_open()) return;
  string line;
  while (getline(f, line)) {
    if (line.empty()) continue;
    stringstream ss(line);
    string sInvId, sClientId, sTripId, sTotal, sPaid;
    getline(ss, sInvId, ',');
    getline(ss, sClientId, ',');
    getline(ss, sTripId, ',');
    getline(ss, sTotal, ',');
    getline(ss, sPaid, ',');

    Client *c = nullptr;
    Trip   *t = nullptr;
    for (const auto &cl : clients)
      if (cl->getId() == stoi(sClientId)) { c = cl.get(); break; }
    for (const auto &tr : trips)
      if (tr->getTripId() == stoi(sTripId)) { t = tr.get(); break; }

    if (c && t) {
      int invId = stoi(sInvId);
      auto inv = make_unique<Invoice>(invId, c, t, stod(sTotal), sPaid == "1");
      if (invId >= counter) counter = invId + 1;
      invoices.push_back(move(inv));
    } else {
      cout << "[DB] Warning: skipping invoice " << sInvId
           << " (missing client/trip reference).\n";
    }
  }
  if (!invoices.empty())
    cout << "[DB] Loaded " << invoices.size() << " invoice(s).\n";
}

void deleteClient(vector<unique_ptr<Client>> &clients,
                  const vector<unique_ptr<Invoice>> &invoices) {
  if (clients.empty()) { cout << "[ERROR] No clients to delete.\n"; return; }

  cout << "\nRegistered Clients:\n";
  for (const auto &c : clients) c->display();

  int id;
  cout << "Enter Client ID to delete: ";
  cin >> id;

  for (const auto &inv : invoices)
    if (inv->getClientId() == id) {
      cout << "[ERROR] Cannot delete client — an invoice references this client.\n";
      return;
    }

  for (auto it = clients.begin(); it != clients.end(); ++it)
    if ((*it)->getId() == id) {
      clients.erase(it);
      saveClients(clients);
      cout << "[SUCCESS] Client " << id << " deleted.\n";
      return;
    }

  cout << "[ERROR] No client found with ID " << id << ".\n";
}

void deleteTrip(vector<unique_ptr<Trip>> &trips,
                const vector<unique_ptr<Invoice>> &invoices) {
  if (trips.empty()) { cout << "[ERROR] No trips to delete.\n"; return; }

  cout << "\nLogged Trips:\n";
  for (const auto &t : trips) t->display();

  int id;
  cout << "Enter Trip ID to delete: ";
  cin >> id;

  for (const auto &inv : invoices)
    if (inv->getTripId() == id) {
      cout << "[ERROR] Cannot delete trip — an invoice references this trip.\n";
      return;
    }

  for (auto it = trips.begin(); it != trips.end(); ++it)
    if ((*it)->getTripId() == id) {
      trips.erase(it);
      saveTrips(trips);
      cout << "[SUCCESS] Trip " << id << " deleted.\n";
      return;
    }

  cout << "[ERROR] No trip found with ID " << id << ".\n";
}

void deleteInvoice(vector<unique_ptr<Invoice>> &invoices) {
  if (invoices.empty()) { cout << "[ERROR] No invoices to delete.\n"; return; }

  cout << "\nAll Invoices:\n";
  for (const auto &inv : invoices) {
    cout << fixed << setprecision(2);
    cout << "  [ID: " << inv->getInvoiceId() << "]  Rs." << inv->getTotal()
         << "  —  " << (inv->getPaymentStatus() ? "PAID" : "PENDING") << "\n";
  }

  int id;
  cout << "Enter Invoice ID to delete: ";
  cin >> id;

  for (auto it = invoices.begin(); it != invoices.end(); ++it)
    if ((*it)->getInvoiceId() == id) {
      invoices.erase(it);
      saveInvoices(invoices);
      cout << "[SUCCESS] Invoice " << id << " deleted.\n";
      return;
    }

  cout << "[ERROR] No invoice found with ID " << id << ".\n";
}


double getNumber(const string &prompt) {
  double value;
  cout << prompt;
  while (!(cin >> value)) {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "[ERROR] Invalid input. Please enter a number.\n" << prompt;
  }
  return value;
}

int getInt(const string &prompt) {
  int value;
  cout << prompt;
  while (!(cin >> value)) {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "[ERROR] Invalid input. Please enter an integer.\n" << prompt;
  }
  return value;
}

int main() {
  vector<unique_ptr<Client>>  clients;
  vector<unique_ptr<Trip>>    trips;
  vector<unique_ptr<Invoice>> invoices;

  int clientCounter  = 101;
  int tripCounter    = 501;
  int invoiceCounter = 1001;

  cout << "***************************************************\n";
  cout << "* Transport Billing & Invoice Management System   *\n";
  cout << "***************************************************\n\n";

  loadClients(clients, clientCounter);
  loadTrips(trips, tripCounter);
  loadInvoices(invoices, clients, trips, invoiceCounter);

  int choice;
  do {
    cout << "\n--- MAIN MENU ---\n";
    cout << "1. Register New Client\n";
    cout << "2. Log New Transport Trip\n";
    cout << "3. Generate Invoice\n";
    cout << "4. Process Payment\n";
    cout << "5. View All Invoices\n";
    cout << "6. Delete Client\n";
    cout << "7. Delete Trip\n";
    cout << "8. Delete Invoice\n";
    cout << "9. Exit\n";
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
      clients.push_back(make_unique<Client>(clientCounter, name, contact));
      cout << "[SUCCESS] Client registered with ID: " << clientCounter++ << "\n";
      saveClients(clients);

    } else if (choice == 2) {
      if (clients.empty()) { cout << "[ERROR] Please register a client first!\n"; continue; }
      double dist  = getNumber("Enter Distance (km): ");
      double rate  = getNumber("Enter Rate per km (Rs): ");
      double base  = getNumber("Enter Base Fare (Rs): ");
      double extra = getNumber("Enter Extra Charges/Tolls (Rs): ");
      trips.push_back(make_unique<Trip>(tripCounter, dist, rate, base, extra));
      cout << "[SUCCESS] Trip logged with ID: " << tripCounter++ << "\n";
      saveTrips(trips);

    } else if (choice == 3) {
      if (clients.empty() || trips.empty()) {
        cout << "[ERROR] Ensure at least one client and one trip exist.\n"; continue;
      }
      cout << "\nAvailable Clients:\n";
      for (const auto &c : clients) c->display();
      int clientId = getInt("Select Client ID: ");

      Client *selectedClient = nullptr;
      for (const auto &c : clients)
        if (c->getId() == clientId) { selectedClient = c.get(); break; }
      if (!selectedClient) { cout << "[ERROR] No client found with ID " << clientId << ".\n"; continue; }

      cout << "\nAvailable Trips:\n";
      for (const auto &t : trips) t->display();
      int tripId = getInt("Select Trip ID: ");

      Trip *selectedTrip = nullptr;
      for (const auto &t : trips)
        if (t->getTripId() == tripId) { selectedTrip = t.get(); break; }
      if (!selectedTrip) { cout << "[ERROR] No trip found with ID " << tripId << ".\n"; continue; }

      auto newInvoice = make_unique<Invoice>(invoiceCounter, selectedClient, selectedTrip);
      newInvoice->generateInvoice();
      newInvoice->saveToFile();
      invoices.push_back(move(newInvoice));
      invoiceCounter++;
      saveInvoices(invoices);

    } else if (choice == 4) {
      if (invoices.empty()) { cout << "[ERROR] No invoices generated yet!\n"; continue; }

      bool anyPending = false;
      cout << "\nPending Invoices:\n";
      for (const auto &inv : invoices) {
        if (!inv->getPaymentStatus()) {
          cout << fixed << setprecision(2);
          cout << "  [ID: " << inv->getInvoiceId() << "]  Rs." << inv->getTotal() << "  —  PENDING\n";
          anyPending = true;
        }
      }
      if (!anyPending) { cout << "[INFO] All invoices are already paid.\n"; continue; }

      int invId = getInt("Enter Invoice ID to pay: ");
      Invoice *target = nullptr;
      for (const auto &inv : invoices)
        if (inv->getInvoiceId() == invId) { target = inv.get(); break; }

      if (!target) { cout << "[ERROR] No invoice found with ID " << invId << ".\n"; continue; }
      if (target->getPaymentStatus()) { cout << "[INFO] Invoice #" << invId << " is already paid.\n"; continue; }

      cout << fixed << setprecision(2);
      cout << "Total Due: Rs." << target->getTotal() << "\n";
      double paymentAmt = getNumber("Enter payment amount: Rs.");

      Payment pay(target, paymentAmt);
      pay.processPayment();

      if (target->getPaymentStatus()) {
        target->saveToFile();
        saveInvoices(invoices);
      }

    } else if (choice == 5) {
      if (invoices.empty()) { cout << "[INFO] No invoices yet.\n"; continue; }
      for (const auto &inv : invoices) inv->generateInvoice();

    } else if (choice == 6) {
      deleteClient(clients, invoices);

    } else if (choice == 7) {
      deleteTrip(trips, invoices);

    } else if (choice == 8) {
      deleteInvoice(invoices);

    } else if (choice != 9) {
      cout << "[ERROR] Invalid choice. Try again.\n";
    }

  } while (choice != 9);

  cout << "\nExiting system. All data saved. Goodbye!\n";
  return 0;
}
