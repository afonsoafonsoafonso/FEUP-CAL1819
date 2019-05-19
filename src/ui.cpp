//#include "src/options/Option.h"
#include "Request.h"
#include "Vehicle.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define MIN_OPTION 1
#define MAX_OPTION 5

bool check_valid_option(int option) {
  return (option >= MIN_OPTION && option <= MAX_OPTION);
}

int get_option() {
  int option;
  cin >> option;
  if (!check_valid_option(option)) {
    cout << "Invalid option. Exiting..." << endl;
    exit(1);
  }
  return option;
}

void print_main_menu() {
  cout << "Please pick one of the following options: " << endl;

  cout << "1. Run" << endl;
  cout << "2. Add vehicles;" << endl;
  cout << "3. Add request;" << endl;
  cout << "4. Load previous session;" << endl;
  cout << "5. Save current session;" << endl;
}

int get_node_info() {
  int id;
  cout << "Insert the node's ID: ";
  cin >> id;
  cin.clear();
  cin.ignore(999999, '\n');
  cout << "Node successfully added" << endl;
  return id;
}

void get_vehicle_info(Vehicle<string> vehicle) {
  string specialty;
  cout << "What is the type of vehicle? ";
  cin.clear();
  cin.ignore(999999, '\n');
  getline(cin, specialty);

  vehicle.setSpecialty(specialty);
  cout << "Vehicle successfully created" << endl;
}

Request<int> get_request() {
  int origin_id, destination_id;
  string specialty;
  cout << "-> Origin node" << endl;
  origin_id = get_node_info();
  cout << "-> Destination node" << endl;
  destination_id = get_node_info();
  cout << "What is the specialty? ";
  getline(cin, specialty);
  Request<int> request(origin_id, destination_id, specialty);
  return request;
}

int main() {
  string specialty;
  float latitude, longitude;
  vector<Request<int>> requests;
  vector<Vehicle<string>> vehicles;

  cout << "Welcome!" << endl;

  while (true) {
    print_main_menu();
    int option = get_option();

    switch (option) {
    case 2: {
      Vehicle<string> vehicle;
      get_vehicle_info(vehicle);
      vehicles.push_back(vehicle);
      break;
    }
    case 3: {
      requests.push_back(get_request());
      break;
    }
    }
  }
}
