#pragma comment(lib, "../x64/debug/OS11_HTAPI.lib")
#include <sstream>
#include <string>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* oldPayload, const char* newPayload, bool inSuccess);
wchar_t* getWChar(const char* c);
string incrementPayload(char* str);

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	try
	{
		wchar_t* fileName = getWChar(argv[1]);

		ht::HtHandle* ht = ht::open(fileName, true);
		if (ht)
			cout << "\nFileMapping open" << endl;
		else
			throw "error";


		HANDLE log_file_handler = CreateFile(L"../files/update.log", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (log_file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "error open log file: " << endl;
			system("pause");
			return 0;
		}


		char* elementKey = new char[2];
		bool res;

		while (true)
		{
			elementKey[0] = '0' + rand() % 50;
			elementKey[1] = '\0';

			ht::Element* element = ht::createGetElement(elementKey, 2);
			ht::Element* getElement = ht::get(ht, element);

			if (getElement)
			{
				cout << " Key-Ht: " << *(static_cast<const char*>(getElement->key)) << endl<< " New payload: " << *(static_cast<const char*>(getElement->payload)) << endl;
				string oldPayload(static_cast<const char*>(getElement->payload), getElement->payloadLength);
				string newPayload = incrementPayload((char*)getElement->payload);

				if (update(ht, getElement, newPayload.c_str(), newPayload.length() + 1))
				{
					cout << "Open file ht" << endl;
					res = true;
				}
				else
				{
					cout << "Good update" << endl;
					res = false;
				}

				WriteRowInLog(log_file_handler, elementKey, oldPayload.c_str(), newPayload.c_str(), res);
			}
			else
			{
				cout << "Error" << endl;
				res = false;

				WriteRowInLog(log_file_handler, elementKey, "-", "-", res);
			}

			delete element;
			Sleep(1000);
		}

		delete[] elementKey;
	}



	catch (const char* msg)
	{
		cout << msg << endl;
	}
}



void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* oldPayload, const char* newPayload, bool inSuccess)
{
	try
	{

		string logmessage("");

		logmessage += "   Operation:Update   Key = "; logmessage += inKey; logmessage += "   Old payload = "; logmessage += oldPayload; logmessage += "   New payload = ";
		logmessage += newPayload;
		inSuccess ? logmessage += "   success\n" : logmessage += "   error\n";


		LARGE_INTEGER file_pointer;
		file_pointer.QuadPart = 0;
		if (!SetFilePointerEx(inLogFileHandler, file_pointer, NULL, FILE_END))
		{
			throw "SetFilePointerEx ... False";
		}

		if (!SetEndOfFile(inLogFileHandler))
		{
			throw "SetEndOfFile ... False";
		}

		DWORD bytesWritten;
		WriteFile(inLogFileHandler, logmessage.c_str(), logmessage.length(), &bytesWritten, NULL);
	}

	catch (const char* msg)
	{
		cout << msg << endl;
	}
}

wchar_t* getWChar(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}

string incrementPayload(char* str)
{
	int num = atoi(str);
	num++;
	stringstream ss;
	ss << num;
	string result = ss.str();
	return result;
}