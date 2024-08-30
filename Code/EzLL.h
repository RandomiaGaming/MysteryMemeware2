#include <Windows.h>

struct EzNode {
	void* value;
	EzNode* next;
};
struct EzLL {
	UINT32 count;
	EzNode* head;
	EzNode* enumerator;
};

UINT32 EzLLCount(EzLL* list);

void EzLLEnumStart(EzLL* list);
void* EzLLEnumGet(EzLL* list);
template <typename T>
inline T* EzLLEnumGetAs(EzLL* list) {
	return reinterpret_cast<T*>(EzLLEnumGet(list));
}
void EzLLEnumNext(EzLL* list);

void EzLLAdd(EzLL* list, void* value);
void EzLLRemoveHead(EzLL* list);
void* EzLLGetHead(EzLL* list);
template <typename T>
inline T* EzLLGetHeadAs(EzLL* list) {
	return reinterpret_cast<T*>(EzLLGetHead(list));
}

void EzLLClear(EzLL* list);

void* EzLLGet(EzLL* list, UINT32 index);
template <typename T>
inline T* EzLLGetAs(EzLL* list, UINT32 index) {
	return reinterpret_cast<T*>(EzLLGet(list, index));
}
void EzLLInsert(EzLL* list, void* value, UINT32 index);
void EzLLRemove(EzLL* list, UINT32 index);

BOOL EzLLContains(EzLL* list, void* value);
void EzLLRemove(EzLL* list, void* value);