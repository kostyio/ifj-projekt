/************************************************************************
 * 
 * Compiler implementation for imperative programming language IFJ18
 * 
 * Autors:
 * Sasák Tomáš - xsasak01
 * Venkrbec Tomáš - xvenkr01
 * Krajči Martin - xkrajc21
 * Natália Dižová - xdizov00 
 * 
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"

// precedence table symbols
typedef enum {
	S = 500,	// < shift
	R, 			// > reduce
	E, 			// = equal
	N4,	  		// error number 4
	N2,			// error number 2
	F			// function param
} T_sign;

typedef enum {
	I_plus_minus,		// 0 +-
	I_mul_div,			// 1 */
	I_rel_op,			// 2 RO
	I_comp_op,			// 3 CO
	I_open_par,			// 4 (
	I_close_par,		// 5 )
	I_data,				// 6 i
	I_dollar			// 7 $
} Prec_table_index

// precedence table
int prec_table[8][8] = {
//	   +-  */  RO  CO  (   )   i   $
	{  R,  S,  R,  R,  S,  R,  S,  R  }, // +-
	{  R,  R,  R,  R,  S,  R,  S,  R  }, // */
	{  S,  S,  N4, N4, S,  R,  S,  R  }, // RO (relation operators) < > <= >=
	{  S,  S,  N4, N4, S,  R,  S,  R  }, // CO (compare operators) == !=
	{  S,  S,  S,  S,  S,  E,  S,  N2 }, // (
	{  R,  R,  R,  R,  N2, R,  N2, R  }, // )
	{  R,  R,  R,  R,  N2, R,  N2, R  }, // i
	{  S,  S,  S,  S,  S,  N2, S,  N2 }  // $
};

#define DOLLAR 200	 // $
#define STOP 201 	 // <
#define NONTERM 202  // nonterminal E
tToken tmp_head;

//get precedence table index
int table_index(tToken token){
	switch (token.type){
		case PLUS:
		case MINUS:
			return I_plus_minus;
		case ASTERISK:
		case SLASH:
			return I_mul_div;
		case EQUAL:
		case NOT_EQUAL:
			return I_com_op;
		case LESS_THAN:
		case LESS_EQUAL:
		case GREATER_THAN:
		case GREATER_EQUAL:
			return I_rel_op;
		case OPEN_PARENTH:
			return I_open_par;
		case CLOSE_PARENTH:
			return I_close_par;
		case INTEGER:
		case FLOAT:
		case STRING:
		case ID:
		case IDF:
			return I_data;
		default:
			return I_dollar;
	}
}

void reduce_by_rule(tStack *tmp_stack){
	tToken rule_token;
	rule_token.type = NONTERM;
	tToken head = head_stack(tmp_stack);
	//E->i
	if ( head.type == ID || head.type == INTEGER || head.type == FLOAT || head.type == STRING ){
		PUSHS head;
		pop_stack(tmp_stack);
		push_stack(pData.stack, rule_token);
	}
	else if( head.type == OPEN_PARENTH ){
		//E->(E)
		pop_stack(tmp_stack);
		head = head_stack(tmp_stack);
		if (head.type == NONTERM){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == CLOSE_PARENTH){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
			}
		}
	}
	else if( head.type == NONTERM ){
		pop_stack(tmp_stack);
		head = head_stack(tmp_stack);
		//E->E+E
		if (head.type == PLUS){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//ADDS
				// insert_instr(pData.instrs, ADD);
			}
		}
		//E->E-E
		else if(head.type == MINUS){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//SUBS
				// insert_instr(pData.instrs, SUB);
			}
		}
		//E->E*E
		else if(head.type == ASTERISK){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//MULS
				// insert_instr(pData.instrs, MUL);
			}
		}
		//E->E/E
		else if(head.type == SLASH){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//DIVS
				// insert_instr(pData.instrs, DIV);
			}
		}
		//E->E==E
		else if(head.type == EQUAL){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//EQS
				// insert_instr(pData.instrs, EQ);
				}
			}
		}
		//E->E!=E
		else if(head.type == NOT_EQUAL){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//NOTS
				//EQS
				// insert_instr(pData.instrs, NE);
			}
		}
		//E->E>E
		else if(head.type == GREATER_THAN){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//GTS
				// insert_instr(pData.instrs, GT);
			}
		}
		//E->E<E
		else if(head.type == LESS_THAN){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//LTS
				// insert_instr(pData.instrs, LT);
			}
		}
		//E->E>=E
		else if(head.type == GREATER_EQUAL){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//GTS
				//OR
				//EQS
				// insert_instr(pData.instrs, GE);
			}
		}
		//E->E<=E
		else if(head.type == LESS_EQUAL){
			pop_stack(tmp_stack);
			head = head_stack(tmp_stack);
			if (head.type == NONTERM){
				pop_stack(tmp_stack);
				push_stack(pDatak.stack, rule_token);
				//LTS
				//OR
				//EQS
				// insert_instr(pData.instrs, LE);
			}
		}
	}
	head = head_stack(tmp_stack);
  if (head.type != EMPTY)
		error(SY_ERR);
}

// operation reduce '>'  =========================================================
void opt_reduce(){
	tStack tmp_stack;
	init_stack(tmp_stack);
	do{	//temporary token for saving data from main stack
		tToken tmp = head_stack(pData.stack);
		//push data to temporary stack
		push_stack(tmp_stack, tmp);
		pop_stack(pData.stack);
		tToken head = head_stack();
	}while( head != stop_token );
	pop_stack(pData.stack)
	tmp_head = head_stack(pData.stack);
	reduce_by_rule(&tmp_stack);
}

// operation switch '<'  =========================================================
void opt_switch(){
	tToken stop_token;
	stop_token.type = STOP;
	push_stack(pData.stack, stop_token);
	push_stack(pData.stack, pData.token);
	tmp_head = head_stack(pData.stack);
	GET_TOKEN();
}

// insert token to the end of the list
void insert_list(tTList **list, tToken token){
	if (*list == NULL){
		*list = malloc(sizeof(struct tTList));
		list->param = token;
		list->next = NULL;
	}
	else{
		tTList *tmp = *list->next;
		tTList *prev = tmp;
		while(tmp != NULL){
			prev = tmp;
			tmp = tmp->next;
		}
		prev->next = malloc(sizeof(struct tTList));
		prev->next.param = token;
		prev->next.next = NULL;
	}
}

// copy contents to main stack
void copy_list_to_stack(tTlist *list){
	while(list != NULL){
		push_stack(pData.stack, list->param);
		tTList *tmp = list;
		list = list->next;
		free(tmp);
	}
}

// operation equal '='  ==========================================================
void opt_eq(){
	// temporary list
	tTList *tmp_head = NULL;
	do{
		tToken tmp = head_stack(pData.stack);
		insert_list(&tmp_head, tmp);

		pop_stack(pData.stack);
		tToken head = head_stack();
	}while( head != stop_token );
	pop_stack(pData.stack)
	// push naspat vsetky tokeny z tmp_list
	copy_list_to_stack(&tmp_head);
}

void pars_expression(){
	// push end of stack = dollar
	tToken stack_end;
	stack_end.type = DOLLAR;
	push_stack(pData.stack, stack_end);
	GET_TOKEN();
	while (1) {
		int activ = table_index(pData.token.type);
		tToken top_stack = head_stack(pData.stack);
		int top = table_index(top_stack.type);
		if((prec_table[activ][top])==S)
				opt_switch();
		else if((prec_table[activ][top])==R)
				opt_reduce();
		else if((prec_table[activ][top])==E)
				opt_eq();
		else if((prec_table[activ][top])==N4)
				error(DATA_ERR);
		else
				error(SY_ERR);
	}
}
