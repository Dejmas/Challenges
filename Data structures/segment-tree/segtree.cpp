#include <vector>
#include <cassert>


class SegmentTree {
	public:
        SegmentTree(int n);
        void set(size_t index, int value);
        void build();
        void update(size_t index, int value);
        long sum(int from, int to) const;
        long slow_sum(int from, int to) const;
        void test(int from, int to) const;

        void print() const;

    private:
        size_t m_N;
        std::vector<int> m_data;
};


SegmentTree::SegmentTree(int n) : m_N(n), m_data(2*n) {}

void SegmentTree::set(size_t index, int value) {
    m_data[m_N+index] = value;
}

void SegmentTree::build() {
    for (size_t i = m_N-1; i > 0; -- i) 
        m_data[i] = m_data[2*i] + m_data[2*i+1];
}

void SegmentTree::update(size_t index, int value) {
    m_data[index + m_N] = value;
    size_t idx = (m_N + index) / 2;
    while (idx > 0) {
        m_data[idx] = m_data[2*idx] + m_data[2*idx+1];
        idx /= 2;
    }
}

/// @param from included
/// @param to excluded
/// @return sum of interval from to to
long SegmentTree::sum(int from, int to) const {
    long total = 0;
    from += m_N; to += m_N;
    for (; from < to; from /= 2, to /= 2) {
        if (from % 2 == 1) {
            total += m_data[from ++];
        }
        if (to % 2 == 1) {
            total += m_data[to-1];
        }
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


int main () {
    int N = 13;
    SegmentTree tree(N);
    for (int i = 0; i < N; i ++)
        tree.set(i, i+1);
    tree.build();
    tree.print();

    for (int i = 0; i < N; i ++) {
        for (int j = i; j < N; ++ j) {
            tree.test(i, j);
        }
    }
    return 0;
}