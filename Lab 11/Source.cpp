#include <Windows.h>
#include <string>
#include <sstream>

COLORREF backColor = RGB(255, 255, 255);
HWND hwnds[5]; // дескриптор окн второстепенных макс. число 5
HWND mainHwnd; //дескриптор главного окна
int hwnds_length = 0; //установка начального количество второстепенных окон
HINSTANCE hInst;  //дескриптор экземпляра приложения
int cmd_show; //режим отображения окна

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);//работа с сообщениями
//UINT – 32 - битное беззнаковое целое
//WPARAM – тип для описания wParam (word parameter)
//LPARAM – тип для описания lParam(long parameter)
//Основная функция - аналог int main() в консольном приложении
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, // нужен для запуска окна в режиме командной строки
	int nCmdShow) //режим отображения окна
{
	hInst = hInstance;
	cmd_show = nCmdShow;

	HWND hwnd;
	MSG msg; //для сообщений, отображения текста в окне
	WNDCLASS w; //класс окна

	memset(&w, 0, sizeof(WNDCLASS)); //&w - указатель на блок памяти для заполнения
	w.style = CS_HREDRAW | CS_VREDRAW; //подключение стилей классов: 
									   //CS_HREDRAW - определяет, что окно будет перерисовываться, как только измениться его горизонтальный размер
									   //CS_VREDRAW - определяет, что окно будет перерисовываться, как только измениться его вертикальный размер
	w.lpfnWndProc = WndProc;// указатель на пользовательскую функцию
	w.hInstance = hInstance;  //дескриптор экземпляра
	w.hbrBackground = CreateSolidBrush(backColor); //определение цвета окна
	w.lpszClassName = L"My Class"; // имя класса
	RegisterClass(&w); //ренистрирует класс

	hwnd = CreateWindow(L"My Class", L"Sun's Laba",
		WS_OVERLAPPEDWINDOW, 300, 200, 400, 280, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow); //отображение окна
	UpdateWindow(hwnd); //обновление окна
	mainHwnd = hwnd; //Отобpажает или пpячет окно обpазом, указанным паpаметpом CmdShow
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

std::string intToStringRadix(int n, int radix) {//n - некоторое число, radix - СС
	char* c;
	c = new char[sizeof(n) * 8 + 1];
	itoa(n, c, radix);
	std::string result = c;
	delete(c);
	return result;
}//переводит из одной системы счисления в другую и преобразование в строку

std::string intToStringRadixWidth(int n, int radix, int width) {
	std::string result = intToStringRadix(n, radix);
	while (result.length() < width) {
		result = "0" + result;
	}
	return result;
} //добавляет 0 в 16-ричной СС в каждом блоке цвета

std::string toUpperCase(std::string source) {
	for (int i = 0; i < source.length(); i++) {
		if (source[i] <= 'z' && source[i] >= 'a') {
			source[i] = source[i] - 'a' + 'A';
		}
	}
	return source;
} // перевод прописных букв в заглавные

std::string getColorName(COLORREF color) {
	std::string result = "#";
	result += toUpperCase(intToStringRadixWidth(GetRValue(color), 16, 2));
	result += toUpperCase(intToStringRadixWidth(GetGValue(color), 16, 2));
	result += toUpperCase(intToStringRadixWidth(GetBValue(color), 16, 2));
	return result;
} //запись цвета строкой

void windowSetRandomColor(HWND hwnd) {
	backColor = RGB(rand() % 256, rand() % 256, rand() % 256);
	InvalidateRect(hwnd, NULL, FALSE);
	//рандомное создание цвета
	std::string s = "New color ";
	s += getColorName(backColor);
	std::wstringstream wss;
	wss << s.c_str();
	//создание строки для заголовка
	SetWindowText(hwnd, wss.str().c_str());
}

LONG WINAPI WndProc(HWND hwnd,
	UINT Message, // сообщение, посылаемое ОС
	WPARAM wparam, // параметры
	LPARAM lparam) // сообщений, для последующего обращения
{
	switch (Message)
	{
	case WM_DESTROY: //условие закрытия окна
		if (hwnd == mainHwnd) {
			PostQuitMessage(0); //отправляем WinMain() сообщение WM_QUIT
		}
		break;
	case WM_LBUTTONUP: {
		if (hwnd == mainHwnd) {
			windowSetRandomColor(hwnd);
			std::string s = intToStringRadix(hwnds_length + 1, 10);
			std::wstringstream wss;
			wss << s.c_str();
			if (hwnds_length < 5) {
				hwnds[hwnds_length] = CreateWindow(L"My Class", wss.str().c_str(),
					WS_OVERLAPPEDWINDOW, 700 + 32 * hwnds_length, 200 + 32 * hwnds_length, 400, 280, NULL, NULL, hInst, NULL);
				ShowWindow(hwnds[hwnds_length], cmd_show);
				UpdateWindow(hwnds[hwnds_length]);
				hwnds_length++;
			}
		}
		break;
	}
	case WM_KEYDOWN: {
		if (hwnds_length > 0) {
			hwnds_length--;
			DestroyWindow(hwnds[hwnds_length]); //разрушает указанное окно
			//SendMessage(hwnds[hwnds_length], WM_DESTROY, NULL, NULL);
		}
		break;
	}
	case WM_PAINT: //функции рисования 
		PAINTSTRUCT ps; //объект
		BeginPaint(hwnd, &ps); //начало прорисоки
		FillRect(ps.hdc, &ps.rcPaint, CreateSolidBrush(backColor)); //Заполняет пpямоугольник, используя кисть Brush, до пpавой и нижней гpаниц
		EndPaint(hwnd, &ps); //конец прорисовки
		break;
	case WM_CLOSE:
		if (hwnd == mainHwnd) {
			if (IDOK == MessageBox(hwnd, L"Exit programm?", L"Close",
				MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
				SendMessage(hwnd, WM_DESTROY, NULL, NULL);
		}
		break;
	default: return DefWindowProc(hwnd, Message, wparam, lparam);
		//Обеспечивает стандаpтную обpаботку сообщений для сообщений, котоpые явно не обpабатываются пpикладной задачей
	}
	return 0;
}

