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

package com.gensym.rethink.demo;

import com.gensym.rethink.Branch;
import com.gensym.rethink.Path;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


network BranchDemo
{
    @display("bgb=480,267");
    submodules:
        source: Source {
            @display("p=63,127");
            interArrivalTime = exponential(1s);
        }
        branch: Branch {
            @display("p=235,127");
        }
        sink1: Sink {
            @display("p=398,62");
        }
        sink2: Sink {
            @display("p=398,187");
        }
    connections:
        branch.out++ --> Path {  branch_proportion = 0.7; } --> sink1.in++;
        branch.out++ --> Path {  branch_proportion = 0.3; } --> sink2.in++;
        source.out --> branch.in++;
}
