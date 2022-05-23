#include <iostream>
#include <iomanip>
#include <optional>
#include <tuple>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

// Task 1
// Создайте структуру Person с 3 полями: фамилия, имя, отчество. Поле отчество должно быть
//опционального типа, т.к. не у всех людей есть отчество. Перегрузите оператор вывода данных
//для этой структуры. Также перегрузите операторы < и == (используйте tie).

class Person {
private:
    std::string FirstName;
    std::string SecondName;
    std::optional < std::string> Patronymic;
public:
    Person(const std::string FName, const std::string SName, const std::optional < std::string> Ptmc) : FirstName(FName), SecondName(SName), Patronymic(Ptmc) {};
    std::string getSecondName() const { return SecondName; };
    friend std::ostream& operator<< (std::ostream& out, const Person& person);
    friend bool operator< (const Person& persL, const Person& persR);
    friend bool operator== (const Person& persL, const Person& persR);
};


std::ostream& operator<< (std::ostream& out, const Person& person) {
    out << std::setw(12) << person.SecondName << " " << std::setw(9) << person.FirstName << " " << std::setw(14) << person.Patronymic.value_or("");
    return out;
}

bool operator<(const Person& persL, const Person& persR) {
    return std::tie(persL.SecondName, persL.FirstName, persL.Patronymic) < std::tie(persR.SecondName, persR.FirstName, persR.Patronymic);
}

bool operator==(const Person& persL, const Person& persR) {
    return std::tie(persL.SecondName, persL.FirstName, persL.Patronymic) == std::tie(persR.SecondName, persR.FirstName, persR.Patronymic);
}

//Создайте структуру PhoneNumber с 4 полями
// код страны (целое число)
//· код города(целое число)
//· номер(строка)
//· добавочный номер (целое число, опциональный тип)
//Для этой структуры перегрузите оператор вывода. Необходимо, чтобы номер телефона выводился в
//формате: +7(911)1234567 12, где 7 – это номер страны, 911 – номер города, 1234567 – номер, 12 –
//добавочный номер. Если добавочного номера нет, то его выводить не надо. Также перегрузите
//операторы < и == (используйте tie)

class PhoneNumber {
private:
    uint16_t countryCode, cityNode;
    std::string number;
    std::optional<uint16_t> extension;

public:
    PhoneNumber() = default;
    PhoneNumber(uint16_t couC, uint16_t citC, const std::string& num, const std::optional<uint16_t>& ext) : countryCode(couC), cityNode(citC), number(num), extension(ext)
    {};
    friend bool operator< (const PhoneNumber& phNumL, const PhoneNumber& phNumR);
    friend std::ostream& operator<< (std::ostream& out, const PhoneNumber& phNum);
};

std::ostream& operator<<(std::ostream& out, const PhoneNumber& phNum) {
    out << "+" << phNum.countryCode << "(" << phNum.cityNode << ")" << phNum.number;
    if (phNum.extension)
        out << " " << phNum.extension.value();
    return out;
}

bool operator<(const PhoneNumber& phNumL, const PhoneNumber& phNumR) {
    return std::tie(phNumL.countryCode, phNumL.cityNode, phNumL.number, phNumL.extension) < std::tie(phNumR.countryCode, phNumR.cityNode, phNumR.number, phNumR.extension);
}


//Создайте класс PhoneBook, который будет в контейнере
//хранить пары: Человек – Номер телефона. Конструктор этого
//класса должен принимать параметр типа ifstream – поток
//данных, полученных из файла. В теле конструктора происходит
//считывание данных из файла и заполнение контейнера. Класс
//PhoneBook должен содержать перегруженный оператор вывода,
//для вывода всех данных из контейнера в консоль.

class PhoneBook {
private:
    std::vector<std::pair<Person, PhoneNumber>> record;
public:
    PhoneBook(std::ifstream& fstr);
    void SortByName();
    void SortByPhone();
    std::tuple<std::string, PhoneNumber> GetPhoneNumber(const std::string& surname);
    void ChangePhoneNumber(const Person& person, const PhoneNumber& newPhNum);
    friend std::ostream& operator<< (std::ostream& out, PhoneBook phBook);
};

class Less {
private:
    bool sortByPhone;
public:
    Less(bool flag = false) : sortByPhone(flag) {};
    bool operator() (const std::pair<Person, PhoneNumber>& left, const std::pair<Person, PhoneNumber>& right) {
        if (left.first == right.first || sortByPhone)
            return left.second < right.second;
        else return left.first < right.first;
    }
};

PhoneBook::PhoneBook(std::ifstream& fstr)
{
	std::string surname, FirsName;
	std::optional<std::string>	patronymic;
	uint16_t SecondName, cityNode;
	std::string number;
	std::optional<std::string> ext;
	std::optional<uint16_t> extension;
	if (fstr.is_open()) {
		while (fstr >> surname >> FirsName >> patronymic.emplace() >> SecondName >> cityNode >> number >> ext.emplace()) {
			if (patronymic.has_value())
				if (patronymic.value() == "-")
					patronymic = std::nullopt;
			Person person(surname, FirsName, patronymic);
			if (ext.has_value())
				if (ext.value() == "-")
					extension = std::nullopt;
				else
					extension.emplace() = std::stoi(ext.value());

			PhoneNumber phoneNum(SecondName, cityNode, number, extension);

			record.push_back(std::make_pair(person, phoneNum));
		}
	}
	fstr.close();
}

void PhoneBook::SortByName() {
	Less less;
	std::sort(record.begin(), record.end(), less);
}

void PhoneBook::SortByPhone() {
	Less less(true);
	std::sort(record.begin(), record.end(), less);
}

std::tuple<std::string, PhoneNumber> PhoneBook::GetPhoneNumber(const std::string& surname) {
	PhoneNumber number;
	std::string res = "";
	uint8_t count = 0;
	std::for_each(record.begin(), record.end(),
		[&](const auto& rec) {
			if (rec.first.getSecondName() == surname) {
				number = rec.second;
				count++;
			}
		});
	if (count == 0) res = "not found";
	else if (count > 1) res = "found more than 1";
	return std::make_tuple(res, number);
}

void PhoneBook::ChangePhoneNumber(const Person& person, const PhoneNumber& newPhNum)
{
	auto find_rec = std::find_if(record.begin(), record.end(),
		[&person](const auto& rec) {
			return rec.first == person;
		});
	if (find_rec != record.end()) find_rec->second = newPhNum;
}

std::ostream& operator<<(std::ostream& out, PhoneBook phBook)
{
	for (const auto& [person, number] : phBook.record) {
		out << person << std::setw(5) << number << std::endl;
	}
	return out;
}

int main() {
	std::ifstream file("... / HW3_1.txt");
	PhoneBook book(file);
	std::cout << book;
	std::cout << "------SortByPhone-------" << std::endl;
	book.SortByPhone();
	std::cout << book;
	std::cout << "------SortByName--------" << std::endl;
	book.SortByName();
	std::cout << book;
	std::cout << "-----GetPhoneNumber-----" << std::endl;
		auto print_phone_number = [&book](const std::string& surname) {
			std::cout << surname << "\t";
		auto answer = book.GetPhoneNumber(surname);
		if (std::get<0>(answer).empty())
			std::cout << std::get<1>(answer);
		else
			std::cout << std::get<0>(answer);
		std::cout << std::endl;
	};
	// вызовы лямбды
	print_phone_number("Ivanov");
	print_phone_number("Petrov");
	std::cout << "----ChangePhoneNumber----" << std::endl;
	book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" },
		PhoneNumber{ 7, 123, "15344458", std::nullopt });
	book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" },
		PhoneNumber{ 16, 465, "9155448", 13 });
	std::cout << book;
}
