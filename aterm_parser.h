/*
 * \brief  Simple Aterm parser
 * \author Emery Hemingway
 * \date   2015-03-13
 */

#ifndef _ATERM_PARSER_H_
#define _ATERM_PARSER_H_

#include <exception.h>


namespace Aterm {
	class Parser;
}

class Aterm::Parser
{

	private:

		enum { MAX_DEPTH = 32 };

		enum State : char { NULL = 0, TUPLE = ')', LIST = ']' };

		char const *_pos;
		size_t      _len;
		int         _depth;
		State       _state[MAX_DEPTH];

		void push(State state)
		{
			if (_depth == MAX_DEPTH) throw Overflow();

			memcpy(&_state[1], &_state[0], sizeof(State)*_depth);
			++_depth; _state[0] = state;
		}

		void pop()
		{
			if (_depth == 1) throw Bad_logic();
			--_depth;
			memcpy(&_state[0], &_state[1], sizeof(State)*_depth);
			advance();
		}

		void advance() { ++_pos; --_len; }

		void check_end()
		{
			if (_len == 0 && _depth == 1) return;
			if ( *_pos == ',')            return advance();
			if (*_pos == _state[0])       return pop();

			throw Malformed_element();
		}

	public:

		struct Exception         : std::exception;
		struct Malformed_element : Exception { };
		struct Wrong_element     : Exception { };
		struct End_of_term       : Exception { };
		struct Bad_logic         : Exception { };
		struct Overflow          : Exception { };
		struct Runoff            : Exception { };

		class String
		{
			private:

				char const *_start;
				int         _len;

			public:

				String() : _start(0), _len(0) { }
				String(char const *start, int len) : _start(start)
				{
					_len = len;
				}

				/**
				 * Return string term as null-terminated string
				 */
				void string(char *dst, int max_len) const
				{
					strncpy(dst, _start, min(_len+1, max_len));
				}

				/**
				 * Return pointer to first character.
				 */
				char const *start() { return _start; };

				/**
				 * Return string length.
				 */
				int len() { return _len; };
		};

		/**
		 * Constructor
		 */
		Parser(char const *start, size_t len)
		: _pos(start), _len(len), _depth(1) { _state[0] = NULL; };

		template <typename FUNC>
		void constructor(char const *name, FUNC const &func)
		{
			size_t len = strlen(name);
			if (len > _len || strcmp(_pos, name, len))
				throw Wrong_element();

			_pos += len;
			_len -= len;

			tuple(func);
		}

		template<typename FUNC>
		void tuple(FUNC const &func)
		{
			if (!_pos || !_len) throw End_of_term();
			if (*_pos != '(') throw Wrong_element();

			advance();
			push(TUPLE);
			func();
			check_end();
		}

		template<typename FUNC>
		void list(FUNC const &func)
		{
			if (!_pos || !_len) throw End_of_term();
			if (*_pos != '[') throw Wrong_element();

			advance();
			int d = _depth;
			push(LIST);
			while (d < _depth) func();
			check_end();
		}

		String string()
		{
			if (*_pos != '"') throw Wrong_element();

			int len = 0;
			char const *pos = _pos + 1;

			while (pos[len] == '\\' || pos[len] != '"') {
				++len;
				if (_len - len == 0) throw Malformed_element();
			}
			_len -= 2 + len;
			_pos += 2 + len;

			check_end();
			return String(pos, len);
		}

		long integer()
		{
			long i;

			while (*_pos >= '0' && *_pos <= '9') {
				i = i*10 + long(*_pos);
				_pos++; _len--;
			}
			check_end();
			return i;
		}

};

#endif
