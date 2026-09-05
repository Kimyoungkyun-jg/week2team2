#pragma once
#include <vector>
#include <initializer_list>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

///////////////////////////////////////
/////////////// TArray ////////////////
///////////////////////////////////////

// 타입이 같은 오브젝트(요소) 시퀀스의 소유권 및 정리 담당
template<typename T>
class TArray
{
private:
    std::vector<T> ArrayData;
    
public:
    // 생성자
    TArray() {}
    TArray(std::initializer_list<T> initList) : ArrayData(initList) {}
    explicit TArray(size_t count) : ArrayData(count) {}
    TArray(size_t count, const T& value) : ArrayData(count, value) {}
    
    // 요소 추가
    void Add(const T& item)
    {
        ArrayData.push_back(item);
    }
    
    // Rvalue Reference      
    void Add(T&& item)
    {
        ArrayData.push_back(std::move(item));
    }

    void push_back(const T& item) { ArrayData.push_back(item); }
    void push_back(T&& item) { ArrayData.push_back(std::move(item)); }
    
    // 요소 삭제           
    void Remove(const T& item)
    {
        ArrayData.erase(std::remove(ArrayData.begin(), ArrayData.end(), item), ArrayData.end());
    }
    
    // 인덱스 기준 삭제        
    void RemoveAt(int idx)
    {
        ArrayData.erase(ArrayData.begin() + idx);
    } 
    
    // 모든 요소 삭제 ( 빈 배열 만들기 )      
    void Empty()
    {
        ArrayData.clear();
    }                    
    void clear() { ArrayData.clear(); }
    
    // Index 접근 조회
    T& operator[](int index)
    {
        return ArrayData[index];
    }
    const T& operator[](int index) const
    {
        return ArrayData[index];
    }
    T& operator[](size_t index)
    {
        return ArrayData[index];
    }
    const T& operator[](size_t index) const
    {
        return ArrayData[index];
    }

    T& back() { return ArrayData.back(); }
    const T& back() const { return ArrayData.back(); }
    void pop_back() { ArrayData.pop_back(); }
    
    // 요소 개수 조회
    int Num() const
    {
        return static_cast<int>(ArrayData.size());
    }
    size_t size() const { return ArrayData.size(); }
    
    // 빈 리스트 여부 조회
    bool IsEmpty() const
    {
        return ArrayData.empty();
    }
    bool empty() const { return ArrayData.empty(); }
    
    // 유효 index 조회
    bool IsValidIndex(int idx) const
    {
        return idx >= 0 && idx < Num();
    }
    
    // 메모리 할당 ( Allocator 최적화 ) 
    void Reserve(int capacity)
    {
        ArrayData.reserve(capacity);
    }
    void reserve(size_t capacity) { ArrayData.reserve(capacity); }
    
    // iterator
    typename std::vector<T>::iterator begin()
    {
        return ArrayData.begin();
    }
    
    typename std::vector<T>::iterator end()
    {
        return ArrayData.end();        
    }
    
    // const iterator ver.
    typename std::vector<T>::const_iterator begin() const
    {
        return ArrayData.begin();
    }
    
    typename std::vector<T>::const_iterator end() const
    {
        return ArrayData.end();        
    }
};

///////////////////////////////////////
/////////////// TSet //////////////////
///////////////////////////////////////

template<typename T>
class TSet 
{
private:
    std::unordered_set<T> SetData;

public:
    TSet() {}
    TSet(std::initializer_list<T> initList) : SetData(initList) {}

    // 요소 추가
    void Add(const T& item)
    {
        SetData.insert(item);
    }
    
    // Rvalue Reference      
    void Add(T&& item)
    {
        SetData.insert(std::move(item));
    }
    
    // KEY 삭제           
    void Remove(const T& item)
    {
        SetData.erase(item);
    }
    
    // 모든 KEY 삭제 ( 빈 SET 만들기 )      
    void Empty()
    {
        SetData.clear();
    }                    
    void clear() { SetData.clear(); }
    
    // KEY 조회
    bool Contains(const T& key) const
    {
        return SetData.find(key) != SetData.end();
    }
    
    // 요소 개수 조회
    int Num() const
    {
        return static_cast<int>(SetData.size());
    }
    size_t size() const { return SetData.size(); }
    
    // 빈 리스트 여부 조회
    bool IsEmpty() const
    {
        return SetData.empty();
    }
    bool empty() const { return SetData.empty(); }

    // 메모리 할당 ( Allocator 최적화 ) 
    void Reserve(int capacity)
    {
        SetData.reserve(capacity);
    }
    
    // iterator
    typename std::unordered_set<T>::iterator begin()
    {
        return SetData.begin();
    }
    
    typename std::unordered_set<T>::iterator end()
    {
        return SetData.end();        
    }
    
    // const iterator ver.
    typename std::unordered_set<T>::const_iterator begin() const
    {
        return SetData.begin();
    }
    
    typename std::unordered_set<T>::const_iterator end() const
    {
        return SetData.end();        
    }
};

///////////////////////////////////////
/////////////// TMap //////////////////
///////////////////////////////////////

template<typename KeyType, typename ValueType>
class TMap 
{
private:
    std::unordered_map<KeyType, ValueType> MapData;
    
public:
    TMap() {}
    TMap(std::initializer_list<std::pair<const KeyType, ValueType>> initList) : MapData(initList) {}
    
    // Key-Value 추가
    void Add(const KeyType& key, const ValueType& value)
    {
        MapData[key] = value;
    }
    
    // Rvalue Reference      
    void Add(const KeyType& key, ValueType&& value)
    {
        MapData[key] = std::move(value);
    }
    
    // KEY 삭제           
    void Remove(const KeyType& key)
    {
        MapData.erase(key);
    }
    
    // 모든 KEY 삭제 ( 빈 SET 만들기 )      
    void Empty()
    {
        MapData.clear();
    }                    
    void clear() { MapData.clear(); }
    
    // KEY 조회
    bool Contains(const KeyType& key) const
    {
        return MapData.find(key) != MapData.end();
    }
    
    // 요소 개수 조회
    int Num() const
    {
        return static_cast<int>(MapData.size());
    }
    size_t size() const { return MapData.size(); }
    
    // 빈 리스트 여부 조회
    bool IsEmpty() const
    {
        return MapData.empty();
    }
    bool empty() const { return MapData.empty(); }

    // 접근
    ValueType& operator[](const KeyType& key)
    {
        return MapData[key];
    }

    // Key 값 찾기
    ValueType* Find(const KeyType& key)
    {   
        auto it = MapData.find(key);
        return ( it != MapData.end())? &(it->second) : nullptr;
        
    }
    // const 반환 Key 값 찾기
    const ValueType* Find(const KeyType& key) const
    {
        auto it = MapData.find(key);
        return ( it != MapData.end())? &(it->second) : nullptr;
    }

    auto find(const KeyType& key) { return MapData.find(key); }
    auto find(const KeyType& key) const { return MapData.find(key); }

    // 메모리 할당 ( Allocator 최적화 ) 
    void Reserve(int capacity)
    {
        MapData.reserve(capacity);
    }
    
    // Iterator
    typename std::unordered_map<KeyType, ValueType>::iterator begin()
    {
        return MapData.begin();
    }
    
    typename std::unordered_map<KeyType, ValueType>::iterator end()
    {
        return MapData.end();        
    }
    
    // const iterator ver.
    typename std::unordered_map<KeyType, ValueType>::const_iterator begin() const
    {
        return MapData.begin();
    }
    
    typename std::unordered_map<KeyType, ValueType>::const_iterator end() const
    {
        return MapData.end();        
    }
};

///////////////////////////////////////
/////////////// TLinkedList ///////////
///////////////////////////////////////

// template<typename T>
// class TLinkedList 
// {
//     private:
    
//     public:
    
// };

///////////////////////////////////////
/////////////// TQueue ////////////////
///////////////////////////////////////

// template<typename T>
// class TQueue 
// {
//     private:

//     public:
    
// };

///////////////////////////////////////
/////////////// TStack ////////////////
///////////////////////////////////////

// template<typename T>
// class TStack 
// {
//     private:

//     public:

// };