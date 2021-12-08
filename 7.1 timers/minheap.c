#include <unistd.h>
#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#include <sys/time.h>
#include <mach/task.h>
#include <mach/mach.h>
#else
#include <time.h>
#endif

#include <iostream>

#include "minheap.h"

static uint32_t
current_time() {
	uint32_t t;
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
	struct timespec ti;
	clock_gettime(CLOCK_MONOTONIC, &ti);
	t = (uint32_t)ti.tv_sec * 1000;
	t += ti.tv_nsec / 1000000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint32_t)tv.tv_sec * 1000;
	t += tv.tv_usec / 1000;
#endif
	return t;
}

int MinHeapTimer::AddTimer(uint32_t expire, TimerHandler cb) {
    int64_t timeout = current_time() + expire;
    TimerNode* node = new TimerNode;
    int id = Count();
    node->id = id;
    node->expire = timeout;
    node->cb = cb;
    node->idx = (int)_heap.size();
    _heap.push_back(node);
    _shiftUp((int)_heap.size() - 1);
    _map.insert(make_pair(id, node));
    return id;
}

bool MinHeapTimer::DelTimer(int id)
{
    auto iter = _map.find(id);
    if (iter == _map.end())
        return false;
    _delNode(iter->second);
    return true;
}

void MinHeapTimer::_delNode(TimerNode *node)
{
    int last = (int)_heap.size() - 1;
    int idx = node->idx;
    if (idx != last) {
        std::swap(_heap[idx], _heap[last]);
        _heap[idx]->idx = idx;
        if (!_shiftDown(idx)) {
            _shiftUp(idx);
        }
    }
    _heap.pop_back();
    _map.erase(node->id);
    delete node;
}

void MinHeapTimer::ExpireTimer()
{
    if (_heap.empty()) return;
    uint32_t now = current_time();
    do {
        TimerNode* node = _heap.front();
        if (now < node->expire)
            break;
        for (int i = 0; i < _heap.size();  i++)
            std::cout << "touch    idx: " << _heap[i]->idx 
                << " id: " << _heap[i]->id << " expire: "
                << _heap[i]->expire << std::endl;
        if (node->cb) {
            node->cb(node);
        }
        _delNode(node);
    } while(!_heap.empty());
}

bool MinHeapTimer::_shiftDown(int pos){
    int last = (int)_heap.size()-1;
    int idx = pos;
    for (;;) {
        int left = 2 * idx + 1;
        if ((left >= last) || (left < 0)) {
            break;
        }
        int min = left; // left child
        int right = left + 1;
        if (right < last && !_lessThan(left, right)) {
            min = right; // right child
        }
        if (!_lessThan(min, idx)) {
            break;
        }
        std::swap(_heap[idx], _heap[min]);
        _heap[idx]->idx = idx;
        _heap[min]->idx = min;
        idx = min;
    }
    return idx > pos;
}

void MinHeapTimer::_shiftUp(int pos)
{
    for (;;) {
        int parent = (pos - 1) / 2; // parent node
        if (parent == pos || !_lessThan(pos, parent)) {
            break;
        }
        std::swap(_heap[parent], _heap[pos]);
        _heap[parent]->idx = parent;
        _heap[pos]->idx = pos;
        pos = parent;
    }
}


void print_hello(TimerNode *te) {
    std::cout <<  "hello world time = " << te->idx << "\t" << te->id << std::endl;
}

int main() {
    MinHeapTimer mht;
    mht.AddTimer(0, print_hello);
    mht.AddTimer(1000, print_hello);
    mht.AddTimer(7000, print_hello);
    mht.AddTimer(2000, print_hello);
    mht.AddTimer(9000, print_hello);
    mht.AddTimer(10000, print_hello);
    mht.AddTimer(6000, print_hello);
    mht.AddTimer(3000, print_hello);
    for (;;) {
        mht.ExpireTimer();
        usleep(10000);
    }
    return 0;
}
