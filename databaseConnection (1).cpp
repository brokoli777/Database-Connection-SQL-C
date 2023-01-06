/*
Group 9
Connor Squires / Alex Trimble / Bregwin Paul Jogi
DBS_211_NGG MILESTONE 2
*/
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <occi.h>
using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};
/* CONNOR SQUIRES SECTION */
int menu(void) {
	int userSel = 0;
	bool valid = false;
	/* MENU OPTIONS */
	cout << "********************HR Menu*********************" << endl;
	cout << "1) Find Employee" << endl
		<< "2) Employees Report" << endl
		<< "3) Add Employee" << endl
		<< "4) Update Employee" << endl
		<< "5) Remove Employee" << endl
		<< "0) Exit" << endl;
	/* LOOP UNTIL VALID USER ENTRY */
	do {
		cout << "Selection: ";
		cin >> userSel;
		if (userSel >= 0 && userSel <= 5) {
			valid = true;
			return userSel;
		}
		cout << "Invalid Selection!" << endl;
	} while (!valid);
}

void displayAllEmployees(Connection* conn) {
	Statement* stmt = nullptr;
	ResultSet* rs = nullptr;

	stmt = conn->createStatement("SELECT e.employeenumber, e.firstname || ' ' || e.lastname, e.email, phone, e.extension, m.firstname || ' ' || m.lastname FROM dbs211_employees e LEFT JOIN dbs211_employees m ON e.reportsto = m.employeenumber LEFT JOIN dbs211_offices o ON o.officecode = e.officecode ORDER BY e.employeenumber");
	rs = stmt->executeQuery();
	
	if (!rs->next()) {
		// if the result set is empty
		cout << "There is no employees’ information to be displayed." << endl;
	}
	else {
		/* DISPLAY EMPLOYEE TITLE */
		cout << endl << setw(6) << left << "E" << setw(20) << left << "Employee Name" << setw(35) << left << "Email" << setw(20) << left << "Phone" << setw(8) << left << "Ext"
			<< setw(15) << left << "Manager" << endl
			<< "------------------------------------------------------------------------------------------------------------------------" << endl;
		/* DISPLAY EMPLOYEE INFO */
		cout << setw(6) << left << rs->getInt(1) << setw(20) << left << rs->getString(2) << setw(35) << left << rs->getString(3)
			<< setw(20) << left << rs->getString(4)  << setw(8) << left << rs->getString(5) << setw(20) << left << rs->getString(6) << endl;
		while (rs->next()) {
			cout << setw(6) << left << rs->getInt(1) << setw(20) << left << rs->getString(2) << setw(35) << left << rs->getString(3)
				<< setw(20) << left << rs->getString(4)  << setw(8) << left << rs->getString(5) << setw(20) << left << rs->getString(6) << endl;
		}
		cout << endl;
	}
}
/* BREGWIN PAUL JOGI SECTION */
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp) {
	Statement* stmt = conn->createStatement();
	ResultSet* rs = stmt->executeQuery("SELECT * FROM dbs211_employees");
	bool employeeNumberPresent = false;
	int EmployeeId[25] = { 0 };
	int count = 0;

	while (rs->next()) {
		EmployeeId[count] = rs->getInt(1);
		count++;
	}

	for (int i = 0; i < count && employeeNumberPresent == false; i++)
	{
		if (employeeNumber == EmployeeId[i]) {
			employeeNumberPresent = true;
		}
	}

	if (employeeNumberPresent) {
		stmt->setSQL("SELECT a.employeeNumber, a.firstName, a.lastName, a.email, c.phone, a.extension , b.firstName || ' ' || b.lastName, a.jobtitle, c.city FROM dbs211_employees a LEFT JOIN dbs211_employees b ON a.reportsto = b.employeenumber LEFT JOIN dbs211_offices c ON c.officecode = a.officecode WHERE a.employeeNumber = :1");
		stmt->setInt(1, employeeNumber);
		rs = stmt->executeQuery();

		while (rs->next()) {
			emp->employeeNumber = rs->getInt(1);
			strcpy(emp->firstName, rs->getString(2).c_str());
			strcpy(emp->lastName, rs->getString(3).c_str());
			strcpy(emp->email, rs->getString(4).c_str());
			strcpy(emp->phone, rs->getString(5).c_str());
			strcpy(emp->extension, rs->getString(6).c_str());
			strcpy(emp->reportsTo, rs->getString(7).c_str());
			strcpy(emp->jobTitle, rs->getString(8).c_str());
			strcpy(emp->city, rs->getString(9).c_str());
		}
		return 1;
	}
	return 0;
}

/* ALEXANDER TRIMBLE SECTION*/
void displayEmployee(Connection* conn, Employee emp) {
	cout << endl;
	cout << "employeeNumber = " << emp.employeeNumber << endl;
	cout << "lastName = " << emp.lastName << endl;
	cout << "firstName = " << emp.firstName << endl;
	cout << "email = " << emp.email << endl;
	cout << "phone = " << emp.phone << endl;
	cout << "extension = " << emp.extension << endl;
	cout << "reportsTo = " << emp.reportsTo << endl;
	cout << "jobTitle = " << emp.jobTitle << endl;
	cout << "city = " << emp.city << endl;
	cout << endl;
}
/* FINAL MILESTONE */
/* CONNOR SQUIRES SECTION */
void insertEmployee(Connection* conn, struct Employee emp) {
	Statement* stmt = nullptr;

	if (findEmployee(conn, emp.employeeNumber, &emp)) {
		cout << "An employee with the same employee number exists." << endl;
	}
	else {
		stmt = conn->createStatement();
		stmt->setSQL("INSERT INTO dbs211_employees (employeenumber, lastname, firstname, email, extension, jobtitle, officecode, reportsto)                                                       VALUES (:1, :2, :3, :4, :5, :6, :7, :8)");
		stmt->setInt(1, emp.employeeNumber);
		stmt->setString(2, emp.lastName);
		stmt->setString(3, emp.firstName);
		stmt->setString(4, emp.email);
		stmt->setString(5, emp.extension);
		stmt->setString(6, emp.jobTitle);
		stmt->setString(7, "1");
		stmt->setString(8, "102"); //THIS IS THE REPORTSTO VALUE --- IF REMOVED THINGS WORK FINE
		stmt->executeUpdate();

		cout << "The new employee is added successfully." << endl;
	}
}
/* BREGWIN PAUL JOGI SECTION */
void updateEmployee(Connection* conn, int employeeNumber) {
	Employee emp;
	char tempPhoneExt[30];

	if (findEmployee(conn, employeeNumber, &emp))
	{
		cout << "Enter a new phone extension : ";
		cin >> tempPhoneExt;

		Statement* stmt = conn->createStatement();

		stmt->setSQL("UPDATE dbs211_employees SET extension = :1 WHERE employeenumber = :2");

		stmt->setString(1, tempPhoneExt);
		stmt->setInt(2, employeeNumber);
		ResultSet* rs = stmt->executeQuery();
	}
	else
	{
		cout << "The entered employee number is invalid" << endl;
	}
}

/* CONNOR SQUIRES SECTION */
int main(void) {
	/* OCCI Variables */
	Environment* env = nullptr;
	Connection* conn = nullptr;
	/* Used Variables */
	string str;
	string user = "dbs211_222g31";
	string pass = "30969227";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";
	/* User Variables*/
	Employee Emp;
	int selection;
	int employNum = 0;
	int valid = 0;
	try
	{
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		/* CALLING MENU / DISPLAY FUNCTIONS */
		do {
			selection = menu();
			if (selection == 1) {
				cout << "Employee Number: ";
				cin >> employNum;
				valid = findEmployee(conn, employNum, &Emp);
				if (!valid) {
					cout << "Employee " << employNum << " does not exist." << endl << endl;
				}
				else
				displayEmployee(conn, Emp);
			}
			else if (selection == 2) {
				displayAllEmployees(conn);
			}
			else if (selection == 3) {
				cout << "Employee Number: ";
				cin >> Emp.employeeNumber;
				cout << "Last Name: ";
				cin >> Emp.lastName;
				cout << "First Name: ";
				cin >> Emp.firstName;
				cout << "Email: ";
				cin >> Emp.email;
				cout << "Extension: ";
				cin >> Emp.extension;
				cout << "Job Title: ";
				cin >> Emp.jobTitle;
				cout << "City: ";
				cin >> Emp.city;

				insertEmployee(conn, Emp);
			}
			else if (selection == 4) {
				cout << "Enter Employee Number: ";
				cin >> employNum;
				updateEmployee(conn, employNum);
			}
		} while (selection != 0);

		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}

	return 0;
}