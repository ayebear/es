// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef EVENTS_H
#define EVENTS_H

#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <deque>

namespace es
{

template <class T>
using EventQueueType = std::deque<T>;

// The following classes are needed for clearAll to work.

struct BaseEventQueue
{
    virtual ~BaseEventQueue() {}
    virtual void clear() = 0;
    virtual size_t size() const = 0;
};

template <class EventType>
struct EventQueue: public BaseEventQueue
{
    EventQueueType<EventType> events;

    void clear()
    {
        events.clear();
    }

    size_t size() const
    {
        return events.size();
    }
};

/*
TODO:
    Add frame counter debugging feature.
        When the frame # of the event != the current frame counter,
            your events are being received too late!
*/

/*
This class can be used to send and receive global events of any type.
To send events:
    es::Events::send(YourOwnEvent("Testing"));
To receive events:
    for (auto& event: es::Events::get<YourOwnEvent>())
        doSomethingWithEvent(event);
To clear all events:
    es::Events::clearAll();
*/
class Events
{
    public:
        // Returns a reference to the queue of the specified type
        template <class T>
        static EventQueueType<T>& get()
        {
            return getQueue<T>().events;
        }

        // Sends a global event
        // es::Events::send(Type(anything));
        template <class T>
        static void send(const T& event)
        {
            get<T>().push_back(event);
        }

        // Sends a global event (forwards arguments)
        // es::Events::send<Type>(anything);
        template <class T, class... Args>
        static void send(Args&&... args)
        {
            get<T>().emplace_back(std::forward<Args>(args)...);
        }

        // Returns true if there are any events of a certain type
        template <class T>
        static bool exists()
        {
            return !get<T>().empty();
        }

        // Removes events of the specified type
        template <class T>
        static void clear()
        {
            get<T>().clear();
        }

        // Clears messages of all types
        static void clearAll()
        {
            for (auto& eventQueue: getEventQueueTable())
                eventQueue.second->clear();
        }

        // Returns the total number of events
        static size_t getTotal()
        {
            size_t total = 0;
            for (auto& eventQueue: getEventQueueTable())
                total += eventQueue.second->size();
            return total;
        }

    private:
        // Returns instance of correct event queue class
        template <class T>
        static EventQueue<T>& getQueue()
        {
            // Get the pointer to the event queue
            auto& specificEvents = getEventQueueTable()[typeid(T)];

            // Create a new queue if it doesn't exist for this type
            if (!specificEvents)
                specificEvents = std::make_unique<EventQueue<T>>();

            // Return the specific type of event queue (casted from the base class pointer)
            return *(static_cast<EventQueue<T>*>(specificEvents.get()));
        }

        // Table of types to event queues
        using EventQueueTable = std::unordered_map<std::type_index, std::unique_ptr<BaseEventQueue>>;
        static EventQueueTable& getEventQueueTable()
        {
            static EventQueueTable eventQueues;
            return eventQueues;
        }
};

}

#endif
