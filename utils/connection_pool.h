/*================================================================
*
*  文件名称：connection_pool.h
*  创 建 者: mongia
*  创建日期：2020年09月25日
*  描    述：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <assert.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

template <class Connection>
class ConnectionPool {
   public:
    ConnectionPool() {}

    ~ConnectionPool() {}

    void add_connection(Connection* connection) { give_back(connection); }

    Connection* get_connection(int32_t timeout = 0)  // ms
    {
        Connection* connection = NULL;
        std::unique_lock<std::mutex> lock(_mtx);
        while (_pool.empty()) {
            if (timeout <= 0) {
                _non_empty.wait(lock);
            } else {
                if (_non_empty.wait_for(lock,
                                        std::chrono::milliseconds(timeout)) ==
                    std::cv_status::timeout)
                    break;
            }
        }

        if (!_pool.empty()) {
            connection = _pool.front();
            _pool.pop();
        }
        return connection;
    }

    void give_back(Connection* connection) {
        assert(connection);

        std::unique_lock<std::mutex> lock(_mtx);
        _pool.push(connection);
        _non_empty.notify_one();
    }

   private:
    std::queue<Connection*> _pool;
    std::mutex _mtx;
    std::condition_variable _non_empty;
};

template <class Connection>
class ConnectionGuard {
   public:
    explicit ConnectionGuard(ConnectionPool<Connection>* pool)
        : _pool(pool), _conn(NULL) {
        if (_pool != NULL) {
            _conn = _pool->get_connection(100);
        }
    }

    ~ConnectionGuard() {
        if (_pool != NULL && _conn != NULL) {
            _pool->give_back(_conn);
        }
    }

    bool check_available() const { return _pool != NULL && _conn != NULL; }

    Connection* operator->() { return _conn; }

   private:
    ConnectionPool<Connection>* _pool;
    Connection* _conn;
};
