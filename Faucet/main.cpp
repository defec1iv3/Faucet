#define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING

#include <iostream>
#include <random>
#include <ctime>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#define BASE_URL "https://bash.ws/dnsleak/test/"

void performCheck();
void checkLeaks(std::string id);
void parseJSON(std::string json);

int main() {;
	srand(time(NULL));
	try {
		performCheck();
	} catch (nlohmann::json::parse_error e) {
		std::cout << "An error occurred!" << std::endl;
		exit(1); // possibly a loss of connection
	}
}

void performCheck() {
	std::cout << "Checking leaks...\n" << std::endl;
	std::string id = std::to_string((rand() % 9999999 + 1000000));
	for (int i = 1; i <= 10; i++) {
		std::string command = "ping -n 1 " + std::to_string(i) + "." + id + ".bash.ws >NUL";
		system(command.c_str());
	}
	checkLeaks(id);
}


void checkLeaks(std::string id) {
	cpr::Response r = cpr::Get (
		cpr::Url {
			BASE_URL + id + "?json"
		},
		cpr::Header {
			{"Content-Type", "application/json"}
		}
	);
	parseJSON(r.text);
}

void parseJSON(std::string json) {
//	std::cout << json;
	nlohmann::json jsonObject = nlohmann::json::parse(json);
	for (auto &i : jsonObject.items()) {
		if (i.value()["type"] != "conclusion") { // inherent std::string operator==() and operator!=()
			std::cout << "[" << i.key() << "]: " << static_cast<std::string>(i.value()["ip"]) << std::endl;
			if (!i.value()["country"].is_null()) {
				std::cout << "\t[country: " << static_cast<std::string>(i.value()["country"]) << "]";
			}
			if (!i.value()["asn"].is_null()) {
				std::cout << "\t[ASN: " << static_cast<std::string>(i.value()["asn"]) << "]";
			}
		}
		else {
			std::cout << "\nStatus: " << static_cast<std::string>(i.value()["ip"]); // bash.ws JSON responses are odd as it stores DNS leak status in "ip" for last entry.
		}
		std::cout << std::endl;
	}
}