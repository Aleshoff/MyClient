#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include<algorithm>

#pragma warning(disable: 4996)

class MyClient
{
private:
	int p;
	int g;
	int a;
	int key;
	std::vector<std::string> text;
	std::vector<char> alphabet;


	void calculateSecretKey(int openKey)
	{
		long long temp = pow(openKey, a);
		key = temp % p;
		std::cout << key << std::endl;
	}

	void transferToArray(char* ch, std::string str)
	{
		ch[str.length()] = '\0';
		for (int i = 0; i < str.size(); ++i)
		{
			ch[i] = str[i];
		}
	}

	std::string  getOpenKey()
	{
		long long temp = pow(g, a);
		int openKey = temp % p;
		return std::to_string(openKey);
	}

	std::string crypt()
	{
		std::string textCrypted = "";

		for (int i = 0; i < text.size(); ++i)
		{
			for (int j = 0; j < text[i].size(); ++j)
			{
				if (text[i][j] == ' ')
				{
					textCrypted += ' ';
					continue;
				}
				auto it = std::find(alphabet.begin(), alphabet.end(), text[i][j]);
				int index = it - alphabet.begin();
				if (index - key >= 0)
				{
					textCrypted += alphabet[index - key];
					continue;
				}
				else if (alphabet.size() - (key - index) % alphabet.size() == 28)
				{
					textCrypted += alphabet[0];
					continue;
				}
				else
					textCrypted += alphabet[alphabet.size() - ((key - index) % alphabet.size())];	
			}
			textCrypted += '\n';
		}
		std::cout << "\nMesajul trimis:\n\n" << textCrypted;
		return textCrypted;
	}
public:
	MyClient(std::string fileName)
	{
		this->p = 33;
		this->g = 5;
		this->key = 0;
		srand(time(NULL));
		this->a = rand() % 10;
		
		std::ifstream in(fileName);

		while (!in.eof())
		{
			std::string txt = "";
			getline(in, txt);
			std::transform(txt.begin(), txt.end(), txt.begin(), std::tolower);
			this->text.push_back(txt);
		}

		this->alphabet = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '.', ' '};
	}


	void start() 
	{
		WSAData wsaData;
		WORD DLLVersion = MAKEWORD(2, 1);
		if (WSAStartup(DLLVersion, &wsaData) != 0) {
			std::cout << "Error" << std::endl;
			exit(1);
		}

		SOCKADDR_IN addr;
		int sizeofaddr = sizeof(addr);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(1111);
		addr.sin_family = AF_INET;

		SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
		if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
			std::cout << "Error: failed connect to server.\n";
			return;
		}
		std::cout << "Connected!\n";
		std::string ok = getOpenKey();
		char buffer[256];
		strcpy(buffer, ok.c_str());
		send(Connection, buffer, sizeof(buffer), NULL);
		char msg[256];
		recv(Connection, msg, sizeof(msg), NULL);
		std::cout << msg << std::endl;
		calculateSecretKey(std::stoi(msg));
		std::string textCrypted = crypt();
		char cryptedBuffer[16384];
		transferToArray(cryptedBuffer, textCrypted);
		send(Connection, cryptedBuffer, sizeof(cryptedBuffer), NULL);
		
		system("pause");
	}
};

void main(int argc, char* argv[]) {
	
	MyClient client("Text.txt");
	client.start();
}