/*
	Module Name: des.cpp
	Description: simulates discrete event system, by maintain and simulating a priority 		queue
*/
#include "scheduler.h"
#include <list>

// data structure for holding all events
std::list<Event*> event_queue;

void put_event(Event *event, bool pushback = false) {
	/*
		Function Name: put_event
		Arguments:
			Event *event: pointer to event object to be inserted
			bool pushback: whether to push at the end or as per timestamp, default: false
		Returns: void
		Description: Inserts event at the end if pushback is true otherwise just insert 		the event in sorted timestamp order
	*/

	// if pushback is true then insert at end
	if(pushback) {
		event_queue.push_back(event);
		return;
	}

	// otherwise look for correct location so that timestamps are in sorted order 
	// and insert there
	std::list<Event*>::iterator index;
	for(index = event_queue.begin(); index != event_queue.end(); index++) {
		if((*index)->timestamp > event->timestamp) {
			break;
		}
	}
	event_queue.insert(index, event);
}


Event* get_event() {
	/*
		Function Name: get_event
		Returns: Event* : first event in the queue
		Description: gets the event at the front and returns the same.
	*/

	// if no events present in the queue then return null
	if(event_queue.size() == 0) {
		return NULL;
	}

	// return first element
	Event *event = event_queue.front();
	event_queue.pop_front();
	return event;
}

int get_next_event_time() {
	/*
		Function Name: get_next_event_time
		Returns: int: timestamp of next event
		Description: return timestamp of event in front of queue
	*/
	if(event_queue.size() == 0) return -1;
	return event_queue.front()->timestamp;
}