// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <queue>
#include <utility>
#include <optional>
#include <functional>

#include "message/queue.hpp"
#include "message/message.hpp"

using namespace std;


Message::Queue::Queue(function<void()> onfill, function<void()> onempty): onfill(onfill), onempty(onempty) {}


void Message::Queue::enqueue(const Array& message) {
    messages.push(message);
    if (messages.size() == 1 && onfill) onfill();
}

void Message::Queue::dequeue() {
    if (messages.empty()) return;

    messages.pop();
    if (messages.empty() && onempty) onempty();
}


optional<Message::Array> Message::Queue::front() const {
    if (messages.empty()) return nullopt;
    return messages.front();
}
