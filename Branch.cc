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

#include <Branch.h>

#define PROPORTION "branch_proportion"

namespace queueing {

Define_Module(Branch);

Branch::~Branch() {
    free(proportions);
    free(probabilities);
}

void Branch::initialize()
{
    int baseId = gateBaseId("out");
    int size = gateSize("out");
    proportions = (double*) malloc(size * sizeof(double));
    probabilities = (double*) malloc(size * sizeof(double));

    // initialize proportion array, checking each "out" gate
    for (int i = 0; i < size; i++) {
        cGate *gate = this->gate(baseId + i);
        cChannel *channel = gate->getChannel();
        if (channel && channel->hasPar(PROPORTION)) {
            double proportion = channel->par(PROPORTION);
            proportions[i] = proportion;
        }
        else {
            proportions[i] = 0.0;
        }
    }

    unity = proportions[0];
    probabilities[0] = proportions[0];
    for (int i = 1; i < size; i++) {
        unity += proportions[i];
        probabilities[i] = probabilities[i-1] + proportions[i];
    }
    for (int i = 0; i < size; i++)
        probabilities[i] /= unity;
}

void Branch::handleMessage(cMessage *msg)
{
    int outGateIndex = 0;

    // dblrand() produces numbers in [0,1)
    double p = dblrand();
    while (p > probabilities[outGateIndex])
        outGateIndex++;

    send(msg, "out", outGateIndex);
}

} // namespace
