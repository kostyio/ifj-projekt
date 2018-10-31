#include <stdio.h>
#include <malloc.h>
#include "symtable.h"

/**
 * Function initalizes stack. 
 * @param symTable Symbol table which is supposed to initalize.
 **/
void stack_init(tSymTable *symTable)
{
    symTable->next = NULL;
    symTable->root = NULL;
}

/**
 * Function returns first member of stack. If stack is empty, returns NULL.
 * @param symTable Symbol table which head user wants to know.
 * @return Function returns head of symbol table.
 **/
tNode* stack_head(tSymTable *symTable)
{
    return symTable->root;
}

/**
 * Function pops out the first BST from stack. If stack is empty, function returns nothing. But this one frees the BST.
 * @param symTable Symbol table from which is popped first BST.
 * @return Function returns popped stack.
 **/
tSymTable* stack_pop(tSymTable *symTable)
{
    if(symTable->root == NULL)
    {
        return symTable;
    }
    else
    {
        tNode *temp;
        temp = symTable->root;
        symTable = symTable->next;
        free(temp);
        return symTable;
    }
}

/**
 * Function push BST on stack as first member of stack.
 * @param symTable Symbol table where is BST pushed.
 * @param new Which BST is supposed to be pushed.
 * @return Function returns pushed symbolic table.
 **/
tSymTable* stack_push(tSymTable *symTable, tNode *new)
{
    tSymTable *temp;
    stack_init(temp);
    temp->root = new;
    temp->next = symTable;
    symTable = temp;
    return symTable;
}

/**
 * Function creates variable node and returns its pointer.
 * @param id Token with informations about variable.
 * @param dataType Information about which dataType does variable have.
 * @return Function returns pointer to the new node.
 **/
tNode* create_var(tToken id, int dataType)
{
    tNode *temp = (tNode *)malloc(sizeof(struct node));
    //TODO unsucc malloc

    str_copy_string(&(temp->id),&(id.attr.str)); // must be string because its variable identificator
    temp->dataType = dataType;
    temp->instrs = NULL;
    temp->paramsNum = 0;
    temp->lptr = NULL;
    temp->rptr = NULL;

    return temp;
}

/**
 * Function inserts new variable into symbol table.
 * @param root Root of the symbol table (BST).
 * @param id Token with informations about variable.
 * @param dataType Information about which dataType does variable have.
 **/ 
tNode* insert_var(tNode *root, tToken id, int dataType)
{
    if(root == NULL)
    {
        return create_var(id, dataType);
    }
    else if(str_cmp_string(&(id.attr.str),&(root->id)) < 0)
    {
        root->lptr = insert_var(root->lptr, id, dataType);
    }
    else if(str_cmp_string(&(id.attr.str),&(root->id)) > 0)
    {
        root->rptr = insert_var(root->rptr, id, dataType);
    }
    return root;
}

/**
 * Function creates function node and returns its pointer.
 * @param id Token with informations about function.
 * @param paramsNum Number of parameters function takes.
 * @return Function returns pointer to the new node.
 **/
tNode *create_fun(tToken id, int paramsNum)
{
    tNode *temp = (tNode *)malloc(sizeof(struct node));
    //TODO unsucc malloc

    str_copy_string(&(temp->id),&(id.attr.str)); // must be string because its function identificator
    temp->dataType = 0;
    temp->instrs = (tIList *)malloc(sizeof(struct instructionList));
    temp->params = (tParamList *)malloc(sizeof(struct paramList));
    temp->paramsNum = paramsNum;
    temp->lptr = NULL;  
    temp->rptr = NULL;

    return temp;
}

/**
 * Function inserts new function into symbol table.
 * @param root Root of the symbol table (BST).
 * @param id Token with informations about function.
 * @param paramsNum Number of parameters function takes.
 **/ 
tNode* insert_fun(tNode* root, tToken id, int paramsNum)
{
    if(root == NULL)
    {
        return create_fun(id,paramsNum);
    }
    else if(str_cmp_string(&(id.attr.str),&(root->id)) < 0)
    {
        root->lptr = insert_fun(root->lptr, id, paramsNum);
    }
    else if(str_cmp_string(&(id.attr.str),&(root->id)) > 0)
    {
        root->rptr = insert_fun(root->rptr, id, paramsNum);
    }
    return root;
}

/**
 * Function searches for the variable/function inside symbol table tree (BST) and returns its pointer.
 * @warning This function is supposed to be used ONLY for searching inside Function! (local symbol table) 
 * @param root Root of symbol table (BST) where variable or function is searched.
 * @param id Identificator of searched symbol.
 * @return Function returns pointer to the searched variable/function, if it is not found returns NULL.
 **/
tNode *search_local_table(tNode *root, string id)
{
    if(str_cmp_string(&(id),&(root->id)) == 0 || root == NULL)
    {
        return root;
    }
    else if(str_cmp_string(&(id),&(root->id)) < 0)
    {
        return search_local_table(root->lptr, id);
    }
    else 
    {
        return search_local_table(root->rptr, id);
    }
}

/**
 * Function searches for the variable/function inside symbol table and returns its pointer.
 * @warning This function is supposed to be used ONLY for searching inside MAIN (searches WHOLE table, not single tree) 
 * @param root Root of symbol table (BST) where variable or function is searched.
 * @param id Identificator of searched symbol.
 * @return Function returns pointer to the searched variable/function, if it is not found returns NULL.
 **/
tNode *search_global_table(tSymTable *symTable, string id)
{
    tNode *result = NULL;
    if(symTable == NULL)
    {
        return NULL;
    }
    while(symTable != NULL)
    {
        result = search_local_table(symTable->root, id);
        symTable = symTable->next;
    }
    return result;
}

tParamList* insert_param(tParamList *params, string id)
{
    tParamList *temp = (tParamList *)malloc(sizeof(struct paramList));
    // TODO: unsucc malloc
    str_copy_string(&(params->id),&(id));
    temp->next = NULL;
    // If the parameters list is empty
    if(params == NULL)
    {
        return temp;
    }
    // If it is not empty
    // Go to the latest parameter
    while(params->next != NULL)
    {
        params = params->next;
    }
    params->next = temp;
    return params;
}

void free_params(tParamList *params)
{
    if(params == NULL)
    {
        return;
    }

    tParamList *temp;
    while(temp != NULL)
    {
        temp = params->next;
        free(params);
        params = temp;
    }
}

/**
 * Function frees whole tree (every node).
 * @param root Pointer to the tree root node.
 **/
void free_tree(tNode *root)
{
    if(root == NULL)
    {
        return;
    }
    if(root->lptr != NULL)
    {
        free_tree(root->lptr);
    }
    if(root->rptr != NULL)
    {
        free_tree(root->rptr);
    }
    if(root->lptr == NULL && root->rptr == NULL)
    {
        free_params(root->params);
        free_ilist(root->instrs);
        free(root);
    }
}

/**
 * Function frees symTable (every BST).
 * @param symTable Pointer to the table.
 **/
void free_symtable(tSymTable *symTable)
{
    while(symTable != NULL)
    {
        free_tree(symTable->root);
        symTable = symTable->next;
    }    
}