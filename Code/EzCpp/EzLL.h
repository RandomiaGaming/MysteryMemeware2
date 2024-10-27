// Approved 10/25/2024

#pragma once
#include "EzError.h"

template<typename T>
class EzLL final {
private:
	struct Node {
		T _value = { };
		Node* _next = NULL;
		Node* _previous = NULL;
	};

	UINT32 _count = 0;
	Node* _head = NULL;
	Node* _tail = NULL;
	Node* _enumerator = NULL;

	void InternalThrowIfEmpty();
	void InternalThrowOnIndex(UINT32 index);
	void InternalThrowIfReset();
	void InternalRemove(Node* currentNode);
	void InternalInitList(T value);
	void InternalInsertBefore(Node* currentNode, T value);
	void InternalInsertAfter(Node* currentNode, T value);
	Node* InternalGetNthNode(UINT32 index);

public:
	EzLL();
	~EzLL();
	UINT32 Count();
	BOOL IsEmpty();
	void Clear();
	void Remove(T value);
	T* ToArray();

	T GetHead();
	void SetHead(T value);
	void InsertHead(T value);
	void RemoveHead();
	void EnumHead();

	T GetTail();
	void SetTail(T value);
	void InsertTail(T value);
	void RemoveTail();
	void EnumTail();

	T GetAt(UINT32 index);
	void SetAt(UINT32 index, T value);
	void InsertAt(UINT32 index, T value);
	void RemoveAt(UINT32 index);

	BOOL IsEnumReset();
	void EnumReset();
	void EnumTo(UINT32 index);
	void EnumNext();
	void EnumPrevious();
	T EnumGet();
	void EnumSet(T value);
	void EnumInsertBefore(T value);
	void EnumInsertAfter(T value);
	void EnumRemoveGoBack();
	void EnumRemoveGoForward();
};

template<typename T>
void EzLL<T>::InternalThrowIfEmpty() {
	if (IsEmpty()) {
		throw EzError(L"Action cannot be preformed while the EzLL is empty.", __FILE__, __LINE__);
	}
}
template<typename T>
void EzLL<T>::InternalThrowOnIndex(UINT32 index) {
	if (index >= _count) {
		throw EzError(L"index was outside the bounds of the EzLL.", __FILE__, __LINE__);
	}
}
template<typename T>
void EzLL<T>::InternalThrowIfReset() {
	if (IsEnumReset()) {
		throw EzError(L"Action cannot be preformed while enumerator is not set.", __FILE__, __LINE__);
	}
}

template<typename T>
void EzLL<T>::InternalRemove(EzLL<T>::Node* currentNode) {
	if (currentNode->_previous != NULL) {
		if (currentNode->_next == NULL) {
			currentNode->_previous->_next = NULL;
			_tail = currentNode->_previous;
		}
		else {
			currentNode->_previous->_next = currentNode->_next;
		}
	}
	if (currentNode->_next != NULL) {
		if (currentNode->_previous == NULL) {
			currentNode->_next->_previous = NULL;
			_head = currentNode->_next;
		}
		else {
			currentNode->_next->_previous = currentNode->_previous;
		}
	}
	if (_enumerator == currentNode) {
		_enumerator = NULL;
	}
	delete currentNode;
	_count--;
}
template<typename T>
void EzLL<T>::InternalInitList(T value) {
	if (_count > 0) {
		DebugBreak();
	}
	EzLL<T>::Node* newNode = new EzLL<T>::Node();
	newNode->_value = value;
	newNode->_previous = NULL;
	newNode->_next == NULL;
	_head = newNode;
	_tail = newNode;
	_count = 1;
}
template<typename T>
void EzLL<T>::InternalInsertBefore(EzLL<T>::Node* currentNode, T value) {
	EzLL<T>::Node* newNode = new EzLL<T>::Node();
	newNode->_value = value;
	if (currentNode->_previous == NULL) {
		newNode->_previous = NULL;
		_head = newNode;
	}
	else {
		newNode->_previous = currentNode->_previous;
		currentNode->_previous->_next = newNode;
	}
	newNode->_next = currentNode;
	currentNode->_previous = newNode;
	_count++;
}
template<typename T>
void EzLL<T>::InternalInsertAfter(EzLL<T>::Node* currentNode, T value) {
	EzLL<T>::Node* newNode = new EzLL<T>::Node();
	newNode->_value = value;
	if (currentNode->_next == NULL) {
		newNode->_next = NULL;
		_tail = newNode;
	}
	else {
		newNode->_next = currentNode->_next;
		currentNode->_next->_previous = newNode;
	}
	newNode->_previous = currentNode;
	currentNode->_next = newNode;
	_count++;
}
template<typename T>
typename EzLL<T>::Node* EzLL<T>::InternalGetNthNode(UINT32 index) {
	if (index < 0 || index >= _count) {
		DebugBreak();
	}
	EzLL<T>::Node* currentNode = _head;
	for (UINT32 i = 0; i < index; i++) {
		currentNode = currentNode->_next;
	}
	return currentNode;
}

template<typename T>
EzLL<T>::EzLL<T>() {
	_count = 0;
	_head = NULL;
	_tail = NULL;
	_enumerator = NULL;
}
template<typename T>
EzLL<T>::~EzLL<T>() {
	Clear();
}
template<typename T>
UINT32 EzLL<T>::Count() {
	return _count;
}
template<typename T>
BOOL EzLL<T>::IsEmpty() {
	return Count() == 0;
}
template<typename T>
void EzLL<T>::Clear() {
	while (!IsEmpty()) {
		RemoveHead();
	}
}
template<typename T>
void EzLL<T>::Remove(T value) {
	EzLL<T>::Node* currentNode = _head;
	while (currentNode != NULL) {
		if (currentNode->_value == value) {
			InternalRemove(currentNode);
			return;
		}
		currentNode = currentNode->_next;
	}
}
template<typename T>
T* EzLL<T>::ToArray() {
	T* output = new T[_count];
	UINT32 index = 0;
	EzLL<T>::Node* currentNode = _head;
	while (currentNode != NULL) {
		output[index] = currentNode->_value;
		currentNode = currentNode->_next;
		index++;
	}
	return output;
}

template<typename T>
T EzLL<T>::GetHead() {
	InternalThrowIfEmpty();
	return _head->_value;
}
template<typename T>
void EzLL<T>::SetHead(T value) {
	InternalThrowIfEmpty();
	_head->_value = value;
}
template<typename T>
void EzLL<T>::InsertHead(T value) {
	if (IsEmpty()) {
		InternalInitList(value);
	}
	else {
		InternalInsertBefore(_head, value);
	}
}
template<typename T>
void EzLL<T>::RemoveHead() {
	InternalThrowIfEmpty();
	InternalRemove(_head);
}
template<typename T>
void EzLL<T>::EnumHead() {
	_enumerator = _head;
}

template<typename T>
T EzLL<T>::GetTail() {
	InternalThrowIfEmpty();
	return _tail->_value;
}
template<typename T>
void EzLL<T>::SetTail(T value) {
	InternalThrowIfEmpty();
	_tail->_value = value;
}
template<typename T>
void EzLL<T>::InsertTail(T value) {
	if (IsEmpty()) {
		InternalInitList(value);
	}
	else {
		InternalInsertAfter(_tail, value);
	}
}
template<typename T>
void EzLL<T>::RemoveTail() {
	InternalThrowIfEmpty();
	InternalRemove(_tail);
}
template<typename T>
void EzLL<T>::EnumTail() {
	_enumerator = _tail;
}

template<typename T>
T EzLL<T>::GetAt(UINT32 index) {
	InternalThrowOnIndex(index);
	EzLL<T>::Node* currentNode = InternalGetNthNode(index);
	return currentNode->_value;
}
template<typename T>
void EzLL<T>::SetAt(UINT32 index, T value) {
	InternalThrowOnIndex(index);
	EzLL<T>::Node* currentNode = InternalGetNthNode(index);
	currentNode->_value = value;
}
template<typename T>
void EzLL<T>::InsertAt(UINT32 index, T value) {
	if (index == _count) {
		InsertTail(value);
	}
	else {
		InternalThrowOnIndex(index);
		EzLL<T>::Node* currentNode = InternalGetNthNode(index);
		InternalInsertBefore(currentNode, value);
	}
}
template<typename T>
void EzLL<T>::RemoveAt(UINT32 index) {
	InternalThrowOnIndex(index);
	EzLL<T>::Node* currentNode = InternalGetNthNode(index);
	InternalRemove(currentNode);
}

template<typename T>
BOOL EzLL<T>::IsEnumReset() {
	return _enumerator == NULL;
}
template<typename T>
void EzLL<T>::EnumReset() {
	_enumerator = NULL;
}
template<typename T>
void EzLL<T>::EnumTo(UINT32 index) {
	InternalThrowOnIndex(index);
	_enumerator = InternalGetNthNode(index);
}
template<typename T>
void EzLL<T>::EnumNext() {
	InternalThrowIfReset();
	_enumerator = _enumerator->_next;
}
template<typename T>
void EzLL<T>::EnumPrevious() {
	InternalThrowIfReset();
	_enumerator = _enumerator->_previous;
}
template<typename T>
T EzLL<T>::EnumGet() {
	InternalThrowIfReset();
	return _enumerator->_value;
}
template<typename T>
void EzLL<T>::EnumSet(T value) {
	InternalThrowIfReset();
	_enumerator->_value = value;
}
template<typename T>
void EzLL<T>::EnumInsertBefore(T value) {
	InternalThrowIfReset();
	InternalInsertBefore(_enumerator, value);
}
template<typename T>
void EzLL<T>::EnumInsertAfter(T value) {
	InternalThrowIfReset();
	InternalInsertAfter(_enumerator, value);
}
template<typename T>
void EzLL<T>::EnumRemoveGoBack() {
	InternalThrowIfReset();
	EzLL<T>::Node* previous = _enumerator->_previous;
	InternalRemove(_enumerator);
	_enumerator = previous;
}
template<typename T>
void EzLL<T>::EnumRemoveGoForward() {
	InternalThrowIfReset();
	EzLL<T>::Node* next = _enumerator->_next;
	InternalRemove(_enumerator);
	_enumerator = next;
}