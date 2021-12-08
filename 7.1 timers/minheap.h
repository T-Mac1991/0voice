#pragma once

#include <vector>
#include <map>
using namespace std;

typedef void (*TimerHandler) (struct TimerNode *node);

struct TimerNode {
    int idx = 0;
    int id = 0;
    unsigned int expire = 0;
    TimerHandler cb = NULL;
};

class MinHeapTimer {
public:
    MinHeapTimer() {
        _heap.clear();
        _map.clear();
    }
    static inline int Count() {
        return ++_count;
    }

    int AddTimer(uint32_t expire, TimerHandler cb);
    bool DelTimer(int id);
    void ExpireTimer();

private:
    inline bool _lessThan(int lhs, int rhs) {
        return _heap[lhs]->expire < _heap[rhs]->expire;
    }
    bool _shiftDown(int pos);
    void _shiftUp(int pos);
    void _delNode(TimerNode *node);

private:
    vector<TimerNode*>  _heap;
    map<int, TimerNode*> _map;  //为了删除时快速找到结点, 从O(n)->O(1)
    static int _count;
};

int MinHeapTimer::_count = 0;
