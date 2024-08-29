#include "EzLL.h"
#include "EzError.h"


UINT32 EzLLCount(EzLL* list) {
	return list->count;
}

void EzLLEnumStart(EzLL* list) {
	list->enumerator = list->head;
}
void* EzLLEnumGet(EzLL* list) {
	if (list->enumerator == NULL) {
		return NULL;
	}
	return list->enumerator->value;
}
void EzLLEnumNext(EzLL* list) {
	if (list->enumerator == NULL) {
		return;
	}
	list->enumerator = list->enumerator->next;
}

void EzLLAdd(EzLL* list, void* value) {
	EzNode* newNode = new EzNode();
	newNode->value = value;
	newNode->next = list->head;
	list->head = newNode;
	list->count++;
}
void EzLLRemoveHead(EzLL* list) {
	if (list->head == NULL) {
		throw EzError(L"list has no head.", __FILE__, __LINE__);
	}
	EzNode* oldHead = list->head;
	list->head = list->head->next;
	delete oldHead;
	list->count--;
}
void* EzLLGetHead(EzLL* list) {
	if (list->head == NULL) {
		return NULL;
	}
	return list->head->value;
}

void EzLLClear(EzLL* list) {
	EzNode* currentNode = list->head;
	while (currentNode != NULL) {
		EzNode* deleteMe = currentNode;
		currentNode = currentNode->next;
		delete deleteMe;
	}
	list->enumerator = NULL;
	list->head = NULL;
	list->count = 0;
}

void* EzLLGet(EzLL* list, UINT32 index) {
	if (index >= list->count) {
		throw EzError(L"index was outside the bounds of the linked list.", __FILE__, __LINE__);
	}
	EzNode* currentNode = list->head;
	UINT32 shiftCount = index - 1;
	for (UINT32 i = 0; i < shiftCount; i++)
	{
		currentNode = currentNode->next;
	}
	return currentNode->value;
}
void EzLLInsert(EzLL* list, void* value, UINT32 index) {
	if (index > list->count) {
		throw EzError(L"index was outside the bounds of the linked list.", __FILE__, __LINE__);
	}
	EzNode** previousNextNode = &list->head;
	EzNode* currentNode = list->head;
	UINT32 shiftCount = index - 1;
	for (UINT32 i = 0; i < shiftCount; i++)
	{
		currentNode = currentNode->next;
	}
	EzNode* newNode = new EzNode();
	newNode->value = value;
	newNode->next = currentNode;
	*previousNextNode = newNode;
	list->count++;
}
void EzLLRemove(EzLL* list, UINT32 index) {
	if (index >= list->count) {
		throw EzError(L"index was outside the bounds of the linked list.", __FILE__, __LINE__);
	}
	EzNode** previousNextNode = &list->head;
	EzNode* currentNode = list->head;
	UINT32 shiftCount = index - 1;
	for (UINT32 i = 0; i < shiftCount; i++)
	{
		previousNextNode = &currentNode->next;
		currentNode = currentNode->next;
	}
	*previousNextNode = currentNode->next;
	delete currentNode;
	list->count--;
}

BOOL EzLLContains(EzLL* list, void* value) {
	EzNode* currentNode = list->head;
	while (currentNode != NULL) {
		if (currentNode->value == value) {
			return TRUE;
		}
		currentNode = currentNode->next;
	}
	return FALSE;
}
void EzLLRemove(EzLL* list, void* value) {
	EzNode** previousNextNode = &list->head;
	EzNode* currentNode = list->head;
	while (currentNode != NULL) {
		if (currentNode->value == value) {
			EzNode* deleteMe = currentNode;
			*previousNextNode = currentNode->next;
			currentNode = currentNode->next;
			delete deleteMe;
			list->count--;
		}
		else {
			previousNextNode = &currentNode->next;
			currentNode = currentNode->next;
		}
	}
}