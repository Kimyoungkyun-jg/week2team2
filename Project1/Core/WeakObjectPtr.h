#pragma once
#include <cstddef>
#include <cstdint>

// 객체 해제 시 자동으로 nullptr을 반환하는 약한 참조 스마트 포인터
template<typename T>
class TWeakObjectPtr
{
private:
	mutable T* RawPtr = nullptr;
	mutable uint32 ObjectUUID = 0;

public:
	TWeakObjectPtr() = default;

	TWeakObjectPtr(T* InPtr)
		: RawPtr(InPtr), ObjectUUID(InPtr ? InPtr->GetID() : 0)
	{
	}

	TWeakObjectPtr(std::nullptr_t)
		: RawPtr(nullptr), ObjectUUID(0)
	{
	}

	TWeakObjectPtr& operator=(T* InPtr)
	{
		RawPtr = InPtr;
		ObjectUUID = InPtr ? InPtr->GetID() : 0;
		return *this;
	}

	TWeakObjectPtr& operator=(std::nullptr_t)
	{
		RawPtr = nullptr;
		ObjectUUID = 0;
		return *this;
	}

	T* Get() const
	{
		if (RawPtr)
		{
			if (OBJECT.IsValidObject(RawPtr, ObjectUUID))
			{
				return RawPtr;
			}
			// 이미 삭제된 객체이면 포인터 초기화
			RawPtr = nullptr;
			ObjectUUID = 0;
		}
		return nullptr;
	}

	T* operator->() const { return Get(); }
	operator T*() const { return Get(); }
	explicit operator bool() const { return Get() != nullptr; }

	bool operator==(const TWeakObjectPtr& other) const { return Get() == other.Get(); }
	bool operator!=(const TWeakObjectPtr& other) const { return Get() != other.Get(); }
	bool operator==(const T* other) const { return Get() == other; }
	bool operator!=(const T* other) const { return Get() != other; }

	bool IsValid() const { return Get() != nullptr; }
	void Reset() { RawPtr = nullptr; ObjectUUID = 0; }
};

template<typename T>
using TWeakPtr = TWeakObjectPtr<T>;
