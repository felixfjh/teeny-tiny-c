#include "globals.h"

const char *enum_to_str(enum kind_e kind)
{
	switch (kind)
	{
		case ENDF:		return "ENDF";		 break;
		case NEWLINE:	return "NEWLINE";	 break;
		case IDENT:		return "IDENT";		 break;
		case NUMBER:	return "NUMBER";	 break;
		case STRING:	return "STRING";	 break;

		case LABEL:		return "LABEL";		 break;
		case GOTO:		return "GOTO";		 break;
		case PRINT:		return "PRINT";		 break;
		case INPUT:		return "INPUT";		 break;
		case LET:		return "LET";		 break;
		case IF:		return "IF";		 break;
		case THEN:		return "THEN";		 break;
		case ENDIF:		return "ENDIF";		 break;
		case WHILE:		return "WHILE";		 break;
		case REPEAT:	return "REPEAT";	 break;
		case ENDWHILE:	return "ENDWHILE";	 break;
		
		case EQ:		return "EQ";		 break;
		case PLUS:		return "PLUS";		 break;
		case MINUS:		return "MINUS";		 break;
		case ASTK:		return "ASTK";		 break;
		case SLASH:		return "SLASH";		 break;
		case EQEQ:		return "EQEQ";		 break;
		case NOTEQ:		return "NOTEQ";		 break;
		case LT:		return "LT";		 break;
		case LTEQ:		return "LTEQ";		 break;
		case GT:		return "GT";		 break;
		case GTEQ:		return "GTEQ";		 break;
	}
}


int is_keyword(enum kind_e kind)
{
	if (kind >= 101 && kind <= 111)
	{
		return 1; // keywords (should be freed)
	}

	return 0;
}

int is_nis(enum kind_e kind)
{
	if (kind == 1 || kind == 2 || kind == 3)
	{
		return 1; // NUMBER, INDENT, STRING (should be freed)
	}

	return 0;
}

int is_comparison(enum kind_e kind)
{
	if (kind >= 201 && kind <= 211)
	{
		return 1;
	}

	return 0;
}
