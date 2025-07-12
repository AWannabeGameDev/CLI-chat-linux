#include <stdio.h>

#include "util/terminal_text_box.hpp"

void TerminalTextBox::_initTermios(int echo)
{
	tcgetattr(0, &_oldTerminal); /* grab old terminal i/o settings */
	_currentTerminal = _oldTerminal; /* make new settings same as old settings */
	_currentTerminal.c_lflag &= ~ICANON; /* disable buffered i/o */

	if (echo) 
	{
		_currentTerminal.c_lflag |= ECHO; /* set echo mode */
	} 
	else 
	{
		_currentTerminal.c_lflag &= ~ECHO; /* set no echo mode */
	}

	tcsetattr(0, TCSANOW, &_currentTerminal); /* use these new terminal i/o settings now */
}

void TerminalTextBox::_resetTermios() 
{
	tcsetattr(0, TCSANOW, &_oldTerminal);
}

char TerminalTextBox::_getch() 
{
	char ch;
	_initTermios(0);
	ch = getchar();
	_resetTermios();
	return ch;
}

std::string_view TerminalTextBox::getLine()
{
	_cursor = 0;
	_line.clear();
	_erased = false;

	while(true)
	{
		int newCh {_getch()};

		switch(newCh)
		{
			case 127: // backspace
			{
				if(_cursor != 0)
				{
					printf("\b%.*s \b", (unsigned int)(_line.size() - _cursor), _line.data() + _cursor);

					for(int i {0}; i < _line.size() - _cursor; i++)
					{
						printf("\b");
					}

					_cursor--;
					_line.erase(_line.begin() + _cursor);
				}

				break;
			}

			case (int)'\x1b': // arrow key
			{
				_getch(); // skip '['
				int arrow {_getch()};

				if((arrow == (int)'D') && (_cursor != 0)) // left arrow
				{
					printf("\b");
					_cursor--;
				}
				else if((arrow == (int)'C') && (_cursor != _line.size())) // right arrow
				{
					printf("%c", _line[_cursor]);
					_cursor++;
				}

				break;
			}

			case (int)'\n': // enter key
			{
				return std::string_view {_line.data(), _line.size()};
			}

			default:
			{
				printf("%c%.*s", (char)newCh, (unsigned int)(_line.size() - _cursor), _line.data() + _cursor);

				for(int i {0}; i < _line.size() - _cursor; i++)
				{
					printf("\b");
				}

				_cursor++;
				_line.insert(_line.begin() + (_cursor - 1), (char)newCh);
			}
		}
	}
}

void TerminalTextBox::erase()
{
	if(!_erased)
	{
		for(int i {0}; i < _line.size() - _cursor; i++)
		{
			printf(" ");
		}

		for(int i {0}; i < _line.size(); i++)
		{
			printf("\b \b");
		}

		_erased = true;
	}
}

void TerminalTextBox::redraw()
{
	printf("%.*s", (unsigned int)_line.size(), _line.data());

	for(int i {0}; i < _line.size() - _cursor; i++)
	{
		printf("\b");
	}

	_erased = false;
}