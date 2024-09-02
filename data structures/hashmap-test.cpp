#include <iostream>
#include <functional>
#include <cassert>
#include <sstream>
template <typename TKey, typename TValue>
class LinkedList {
	private:
		struct Node {
			Node(const TKey & key, const TValue & value, Node * next = nullptr)
			: mKey(key)
			, mValue(value)
			, mNext(next) {}
			TKey mKey;
			TValue mValue;
			Node * mNext;
		};
	
	public:
		~LinkedList() {
			while (mHead) {
				Node * temp = mHead->mNext;
				delete mHead;
				mHead = temp;
			}
		}
		bool insert(const TKey & key, const TValue & value) {
			if (find(key)) return false;
			mHead = new Node(key, value, mHead);
			return true;
		}
		bool contains(const TKey & key) const {
			return find(key) != nullptr;
		}
		bool setValue(const TKey & key, const TValue & value) {
			Node * it = find(key);
			if (it == nullptr) return false;
			it->mValue = value;
			return true;
		}
		const TValue & getValue(const TKey & key) {
			Node * it = find(key);
			if (it == nullptr) throw 5;
			return it->mValue;
		}
		bool remove(const TKey & key) {
			Node * node = mHead;
			Node * prev = nullptr;
			for (; node; node = node->mNext) {
				if (node->mKey == key) break;
				prev = node;
			}
			if (!node) return false;
			if (prev)
				prev->mNext = node->mNext;
			else 
				mHead = node->mNext;
			delete node;
			return true;
		}
		template <typename Func>
		void forEach(Func func) {
			for (Node * it = mHead; it; it = it->mNext) {
				func(it->mKey, it->mValue);
			}
		} 

	private:
		Node * find(const TKey & key) const {
			for (Node * it = mHead; it; it = it->mNext) {
				if (it->mKey == key) return it;
			}
			return nullptr;
		}

		Node * mHead = nullptr;

};



template <typename TKey, typename TValue, typename Hasher=std::hash<TKey>>
class HashMap {
	public:
		HashMap(size_t cap = 10, Hasher && hasher = Hasher())
		: mTable (new LinkedList<TKey, TValue>[cap])
		, mCapacity(cap)
		, mHashFunc(std::forward<Hasher>(hasher)) {}

		~HashMap() {
			delete [] mTable;
		}

		bool insert (const TKey & key, const TValue & value) {
			if (mNumInserted > 3 * mCapacity / 4) {
				resize(mCapacity * 2 + 1);
			}
			bool res = mTable[getIndex(key)].insert(key, value);
			if (res) mNumInserted ++;
			return res;
		}
		bool contains (const TKey & key) const {
			return mTable[getIndex(key)].contains(key);
		}
		bool setValue (const TKey & key, const TValue & value) {
			return mTable[getIndex(key)].setValue(key, value);
		}
		const TValue & getValue (const TKey & key) const {
			return mTable[getIndex(key)].getValue(key);
		}
		bool remove (const TKey & key) {
			bool res =  mTable[getIndex(key)].remove(key);
			if (res) mNumInserted --;
			return res;
		}

	private:

		void resize(size_t cap) {
			HashMap<TKey, TValue, Hasher> map(cap, Hasher(mHashFunc));
			for (size_t i = 0; i < mCapacity; i ++) {
				mTable[i].forEach([&map](const TKey& key, const TValue & value){
					map.insert(key, value);
				});
			}

			std::swap(mTable, map.mTable);
			std::swap(mCapacity, map.mCapacity);
		}

		size_t getIndex(const TKey & key) const {
			return mHashFunc(key) % mCapacity;
		}

		LinkedList<TKey, TValue> * mTable;
		size_t 	mCapacity;
		size_t	mNumInserted = 0;
		Hasher  mHashFunc;
};

void testInts () {
	HashMap<int, int, std::hash<int>> map {10};
	
	assert(map.contains(0) == false);
	assert(map.insert(0, 0));
	assert(map.contains(0));
	assert(map.getValue(0) == 0);

	assert(map.insert(0, 1) == false);
	assert(map.getValue(0) == 0);

	assert(map.remove(0));
	assert(map.remove(0) == false);
	assert(map.insert(0, 1));
	assert(map.getValue(0) == 1);

	assert(map.setValue(0, 0));
	assert(map.getValue(0) == 0);
}

void sieveOfEratosthenesTest() {
	HashMap<int, bool, std::hash<int>> primes{1};
	int N = 100;
	for (int i = 2; i < N; i ++) {
		if (primes.contains(i)) continue;
		primes.insert(i, true);
		for (int k = 2 * i; k < N; k += i)
			primes.insert(k, false);
	}

	std::stringstream ss;
	for (int i = 2; i < N; i ++) 
		if (primes.contains(i) && primes.getValue(i)) 
			ss << i << " "; 
		
	assert(ss.str() == "2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 ");
}

#include <algorithm>
#include <iterator>
#include <random>

namespace {
	size_t hashStringStupid(std::string_view str) {
		size_t res = 0;
		for (char x : str) res += x;
		return res;
	}

 	size_t hashStringSmart(std::string_view str) {
		size_t res = 5381;
		for (char x : str) res += (res << 5) + x;
		return res;
	}

	std::string randomString(size_t length) {
		constexpr std::string_view abcd = "ABCDEFGHIJKLMN";
		static std::mt19937 rng{std::random_device{}()};
		std::string res;
		std::sample(abcd.begin(), abcd.end(), std::back_inserter(res), length, rng);
		return res;
	}
}

#include <chrono>
class Benchmarker {
	public:
		Benchmarker(const std::string & name)
		: mName(name) 
		, mStart(std::chrono::steady_clock::now()) {}
		~Benchmarker() {
			using namespace std::chrono;
			auto res = duration_cast<milliseconds>(steady_clock::now() - mStart);
			std::cout << "Benchmark " << mName << ": " << res.count() << " ms (time elapsed)\n";
		}
	private:
		std::string mName;
		std::chrono::time_point<std::chrono::steady_clock> mStart;
};
#include <map>
#include <unordered_map>
void testStrings () {
	HashMap<std::string, int, std::function<size_t (std::string)>> mapStupid {10, hashStringStupid};
	HashMap<std::string, int, std::function<size_t (std::string)>> mapSmart {10, hashStringSmart};
	using namespace std::string_literals;
	const size_t N = 1000000;
	const size_t LEN = 6;
	{
		Benchmarker benchmark("stupid string hash");
		for (size_t i = 0; i < N; i ++) 
			mapStupid.insert(randomString(LEN), i);
	}

	{
		Benchmarker benchmark("smart string hash");
		for (size_t i = 0; i < N; i ++) 
			mapSmart.insert(randomString(LEN), i);
	}

	{
		std::map<std::string, int> test;
		Benchmarker benchmark("std::map");
		for (size_t i = 0; i < N; i ++) 
			test.insert({randomString(LEN), i});
	}

	{
		std::unordered_map<std::string, int> test;
		Benchmarker benchmark("std::unordered_map");
		for (size_t i = 0; i < N; i ++) 
			test.insert({randomString(LEN), i});
	}
}

int main () {
	testInts();
	sieveOfEratosthenesTest();
	testStrings();
}
