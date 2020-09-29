// ExpandableHashMap.h
#ifndef EXPANDABLE_HASH_MAP
#define EXPANDABLE_HASH_MAP
#include <vector>
#include <list>
#include <string>
// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> m_hashVec;
    double m_loadFactor;
    int m_associations;
    int m_numBuckets;
    unsigned int bucketNum(const KeyType& key)const;
    void rehash();
};

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::bucketNum(const KeyType& key)const
{
    unsigned int hasher(const KeyType& k);
    unsigned int h = hasher(key)%m_hashVec.size();
    return h;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::rehash()
{
    std::vector<std::list<std::pair<KeyType, ValueType>>> tempVec(m_hashVec);
    reset();
    m_hashVec.resize(tempVec.size()*2);
    m_numBuckets*=2;
    for(int i = 0; i < tempVec.size(); i++)
    {
        auto it = tempVec[i].begin();
        while(it != tempVec[i].end())
        {
            associate(it->first, it->second);
            it++;
        }
    }
}


template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    if(maximumLoadFactor > 0)
    {
        m_loadFactor = maximumLoadFactor;
    }
    m_numBuckets = 8;
    reset();
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    m_hashVec.clear();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    m_hashVec.clear();
    std::list<std::pair<KeyType, ValueType>> tempList;
    std::vector<std::list<std::pair<KeyType, ValueType>>> tempVec(8, tempList);
    m_hashVec = tempVec;
    m_associations = 0;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_associations;
}


template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    int index = bucketNum(key);
    for(auto it = m_hashVec[index].begin(); it != m_hashVec[index].end(); it++)
    {
        if(it->first == key)
        {
            it->second = value;
            return;
        }
    }
    int currSize = size();
    double tempSize = currSize+1.0;
    double tempBucket = m_numBuckets+0.0;
    double currentLoad = tempSize/tempBucket;
    if(currentLoad >= m_loadFactor)
    {
        rehash();
    }
    index = bucketNum(key);
    m_hashVec[index].push_back(std::pair<KeyType, ValueType>(key, value));
    m_associations++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int index = bucketNum(key);
    for(auto it = m_hashVec[index].begin(); it != m_hashVec[index].end(); it++)
    {
        if(it->first == key)
        {
            return &it->second;
        }
    }
    return nullptr;
}

#endif
