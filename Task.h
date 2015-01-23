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

#ifndef TASK_H_
#define TASK_H_

#include <Job.h>

namespace queueing {

class Task: public cSimpleModule {
    public:
        Task();
        virtual ~Task();

    protected:
        simsignal_t droppedSignal;
        simsignal_t queueLengthSignal;
        simsignal_t queueingTimeSignal;
        simsignal_t busySignal;

        Job **jobServiced;
        cQueue queue;
        int capacity;
        bool fifo;
        int length();
        int current_activities;
        int max_activities;

        Job *getFromQueue();

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        // hook functions to (re)define behaviour
        virtual void arrival(Job *job);
        virtual simtime_t startService(Job *job);
        virtual void endService(Job *job);
};

} /* namespace queueing */

#endif /* TASK_H_ */
