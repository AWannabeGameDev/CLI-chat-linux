#ifndef TERMINAL_TEXT_BOX_HPP
#define TERMINAL_TEXT_BOX_HPP

#include <vector>
#include <string_view>
#include <termios.h>

class TerminalTextBox
{
private :
	std::vector<char> _line {};
	int _cursor {0};

	bool _erased {false};

	termios _oldTerminal;
	termios _currentTerminal;

	void _initTermios(int echo);
	void _resetTermios();
	char _getch();

public :
	TerminalTextBox() = default;

	std::string_view getLine();
	void erase();
	void redraw();
};

#endif