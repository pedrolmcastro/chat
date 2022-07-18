// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <queue>
#include <utility>
#include <optional>
#include <functional>

#include "message/message.hpp"


class Message::Queue {
public:
    Queue(std::function<void()> onfill = {}, std::function<void()> onempty = {});
    Queue(Queue&& other) = default;
    ~Queue() = default;


    void enqueue(const Array& message);
    void dequeue();

    std::optional<Array> front() const;

private:
    std::queue<Array> messages;
    std::function<void()> onfill, onempty;
};
