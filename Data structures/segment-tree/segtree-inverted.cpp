#include <vector>
#include <cassert>


class SegmentTree {
	public:
        SegmentTree(int n);
        void set(size_t index, int value);
        void build();
        void modify(int from, int to, int value);
        long query(size_t index) const;
        long slow_sum(int from, int to) const;
        void test(int from, int to) const;

        void print() const;

    private:
        size_t m_N;
        std::vector<int> m_data;
        std::vector<int> m_naive;
};


SegmentTree::SegmentTree(int n) : m_N(n), m_data(2*n) {}

void SegmentTree::set(size_t index, int value) {
    m_data[m_N+index] = value;
}

void SegmentTree::build() {
    for (size_t i = m_N-1; i > 0; -- i) 
        m_data[i] = m_data[2*i] + m_data[2*i+1];
}

void SegmentTree::modify(int from, int to, int value) {
    from += m_N; to += m_N;
    for (; from < to; from /= 2, to /= 2) {
        if (from % 2 == 1) m_data[from ++] += value;
        if (to % 2 == 1) m_data[--to] += value;
    }
}

/// @param from included
/// @param to excluded
/// @return sum of interval from to to
long SegmentTree::query(size_t index) const {
    long total = 0;
    index += m_N;
    for (; index > 0; index /= 2) {
        total += m_data[index];
    }
    return total;
}

#include <iostream>
void SegmentTree::print() const {
    for (size_t i = 0; i < m_data.size(); ++ i) {
        std::cout << " " << m_data[i];
    }
    std::cout << "\n\n";

    size_t width = m_N;
    size_t levelStart = m_N;
    while (width > 0) {
        for (size_t i = levelStart; i < levelStart + width; ++ i) {
            std::cout << " " << m_data[i];
        }
        
        width = width / 2;    
        levelStart -= width;
        std::cout << "\n";
    }
    std::cout << "\n";
}
/*
long SegmentTree::slow_sum(int from, int to) const {
    from += m_N;
    to += m_N;
    long total = 0;
    for (size_t i = from; i < (size_t)to; ++ i) total += m_data[i];
    return total;
}
void SegmentTree::test(int from, int to) const {
    long quick = sum(from, to);
    long slow = slow_sum(from, to);
    if (quick != slow) 
        std::cerr << "wrong sum(" << from << ", " << to <<") = " << slow 
            << ", but returned " << quick << "\n";
}
*/

int main () {
    int N = 13;
    SegmentTree tree(N);
    tree.modify(2, 7, 1);
    tree.modify(3, 6, 2);
    tree.modify(4, 5, 3);
    tree.modify(5, N, 100);
    tree.print();

    for (int i = 0; i < N; i ++)
        std::cout << " " << tree.query(i);
    std::cout << "\n";    
    
    return 0;
}