//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Task.h"
#include <string.h>

#define END_SERVICE "end-service"

namespace queueing {

Define_Module(Task);

Task::Task()
{
    jobServiced = NULL;
}

Task::~Task()
{
    free(jobServiced);
}

void Task::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);
    busySignal = registerSignal("busy");
    emit(busySignal, false);

    fifo = par("fifo");
    capacity = par("capacity");
    queue.setName("queue");

    max_activities = par("max_activities");
    jobServiced = (Job**) malloc(sizeof(Job*) * max_activities);

    for (int i = 0; i < max_activities; i++)
        jobServiced[i] = NULL;
}

void Task::handleMessage(cMessage *msg)
{
    const char* msgName = msg->getName();
    ev << "message: " << msgName << endl;
    if (strcmp(msgName, END_SERVICE) == 0)
    {
        short slot = msg->getKind();
        endService( jobServiced[slot] );

        if (queue.empty())
        {
            delete msg;
            jobServiced[slot] = NULL;
            emit(busySignal, false);
        }
        else
        {
            // re-use current message and slot
            jobServiced[slot] = getFromQueue();
            emit(queueLengthSignal, length());
            simtime_t serviceTime = startService( jobServiced[slot] );
            scheduleAt( simTime()+serviceTime, msg );
        }
    }
    else
    {
        Job *job = check_and_cast<Job *>(msg);
        arrival(job);

        // find next empty slot
        short slot = 0;
        for (slot = 0; slot < max_activities; slot++)
            if (jobServiced[slot] == NULL) break;

        ev << "slot = " << slot << "(max_activities = " << max_activities << ")" << endl;

        if (slot < max_activities)
        {
            // processor was idle
            cMessage *endServiceMsg = new cMessage(END_SERVICE, slot);
            jobServiced[slot] = job;
            emit(busySignal, true);
            simtime_t serviceTime = startService( jobServiced[slot] );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
        else
        {
            // check for container capacity
            if (capacity >=0 && queue.length() >= capacity)
            {
                EV << "Capacity full! Job dropped.\n";
                if (ev.isGUI()) bubble("Dropped!");
                emit(droppedSignal, 1);
                delete job;
                return;
            }
            queue.insert( job );
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
        }
    }
}

Job *Task::getFromQueue()
{
    Job *job;
    if (fifo)
    {
        job = (Job *)queue.pop();
    }
    else
    {
        job = (Job *)queue.back();
        // FIXME this may have bad performance as remove uses linear search
        queue.remove(job);
    }
    return job;
}

int Task::length()
{
    return queue.length();
}

void Task::arrival(Job *job)
{
    job->setTimestamp();
}

simtime_t Task::startService(Job *job)
{
    // gather queueing time statistics
    simtime_t d = simTime() - job->getTimestamp();
    emit(queueingTimeSignal, d);
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    EV << "Starting service of " << job->getName() << endl;
    job->setTimestamp();
    return par("serviceTime").doubleValue();
}

void Task::endService(Job *job)
{
    EV << "Finishing service of " << job->getName() << endl;
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    send(job, "out");
}

void Task::finish()
{
}

} /* namespace queueing */
