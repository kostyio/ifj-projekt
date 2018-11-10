// LEXICAL ANALYSER

#include "str.h"
#include <stdio.h>
/* init state = 0
 * state after white space/comment = 1
 */
enum comment { 	LC = 10,	// line comment
		BC,		// block comment
		END_C,		// end of block comment	
		TMP_C,		// temporary state for BC
		COMM		// line comment after END_C
};
enum number { 	ZERO = 100,	// FS: zero
		IL,		// FS: integer literal
		FL,		// FS: floating literal
		TMP_FL,		// temporary state for FL
		TMP_F_EX,	// temporary state for FL with exponent
		TMP_I_EX,	// temporary state for IL with exponent
		SIGN_F,		// signed exponent for FL
		SIGN_I,		// signed exponent for IL
		FL_EX,		// FS: floating literal with exponent
		IL_EX		// FS: integer literal with exponent
};
enum ident { 	ID_STATE = 1000,// FS: identifier
		ID_F 		// FS: identifier of function
};
enum string { 	SL = 10000,	// FS: string literal
		TMP_SL,		// temporary state for SL
		ES,			// escape sequence
		X_ES,		// hexadecimal escape sequence \xh
		XHH_ES		// hexadecimal escape sequence \xhh
};


// TOKEN STRUCTURE
typedef struct token{
	int type;
	union {
		int i;
		float f;
		string str;
	} attr ;
}tToken;


// ======= ERRORS =======
#define L_ERR     1
#define SY_ERR	  2
#define DEF_ERR	  3 // not defined var,function, redefinition, etc.
#define DATA_ERR  4 // wrong data type operations (string++, etc.)
#define ARG_ERR   5 // calling parameters count != defined parameters count
#define SE_ERR	  6 // other semanthic errors
#define ZERO_DIV  9 // division by zero
#define INT_ERR	  99

// ======= TYPES OF TOKEN =======
#define IDF     0
#define ID      1
#define INTEGER 2
#define FLOAT   3
#define STRING  4

#define END_OF_FILE 5
#define END_OF_LINE 6

// KEYWORDS
#define DEF   10
#define DO    11
#define ELSE  12
#define END   13
#define IF    14
#define NOT   15
#define NIL   16
#define THEN  17
#define WHILE 18

// OPERATORS
#define PLUS          20 // '+'
#define MINUS         21 // '-'
#define ASTERISK      22 // '*'
#define SLASH         23 // '/'
#define ASSIGNMENT    24 // '='
#define EQUAL         25 // '=='
#define LESS_THAN     26 // '<'
#define LESS_EQUAL    27 // '<='
#define GREATER_THAN  28 // '>'
#define GREATER_EQUAL 29 // '>='
#define NOT_EQUAL     30 // '!='

// SPECIAL CHARACTERS
#define OPEN_PARENTH  40 // '('
#define CLOSE_PARENTH 41 // ')'
#define OPEN_BRACKET  42 // '['
#define CLOSE_BRACKET 43 // ']'
#define OPEN_BRACE    44 // '{'
#define CLOSE_BRACE   45 // '}'
#define COMMA         46 // ','

// Function headers
void set_source_file(FILE *f);
struct token get_token();
