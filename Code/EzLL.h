#include <Windows.h>

class EzLL final {
private:
	struct EzNode {
		void* value;
		EzNode* next;
		EzNode* previous;
	};

	UINT32 count;
	EzNode* head;
	EzNode* tail;
	EzNode* enumerator;

public:
	EzLL();

	inline UINT32 Count() const {
		return count;
	}

	inline void EnumToHead() {
		enumerator = head;
	}
	inline void EnumToTail() {
		enumerator = tail;
	}
	inline void* EnumGet() const {
		return enumerator->value;
	}
	template <typename T>
	inline T* EnumGetAs() const {
		return reinterpret_cast<T*>(EzLLEnumGet(list));
	}
	inline void EnumNext() {
		enumerator = enumerator->next;
	}
	inline void EnumPrevious() {
		enumerator = enumerator->previous;
	}

	void AddHead(void* value);
	void AddTail(void* value);
	void RemoveHead();
	void RemoveTail();
	void* GetHead();
	void* GetTail();
	template <typename T>
	inline T* GetHeadAs() {
		return reinterpret_cast<T*>(GetHead(list));
	}
	template <typename T>
	inline T* GetTailAs() {
		return reinterpret_cast<T*>(GetTail(list));
	}

	void Clear();

	void* Get(UINT32 index);
	template <typename T>
	inline T* GetAs(UINT32 index) {
		return reinterpret_cast<T*>(EzLLGet(index));
	}
	void Insert(void* value, UINT32 index);
	void Remove(UINT32 index);

	BOOL Contains(void* value);
	void Remove(void* value);
};