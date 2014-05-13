/*********************************************************************
*
* Software License Agreement (BSD License)
*
*  Copyright (c) 2011, ISR University of Coimbra.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the ISR University of Coimbra nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
* Author: David Portugal, 2011
*********************************************************************/
#include <sstream>
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>

#include "PatrolAgent.h"

#include "algorithms.h"

class Conscientious_Cognitive_Agent: public PatrolAgent {
    
public:
    virtual void run();
    virtual int compute_next_vertex();
};



int Conscientious_Cognitive_Agent::compute_next_vertex() {
  return 0;
}

void Conscientious_Cognitive_Agent::run() {
    
    /* Run Algorithm */
    
    bool inpath = false;
    uint path [dimension];
    uint elem_s_path=0, i_path=0;
  
    while(ros::ok()) {
      
        if (goal_complete) {
        
            if (i_path>0) { //nao faz update no inicio
                //Update Idleness Table:
                update_idleness();
                current_vertex = next_vertex;
            }
    
            if (inpath){
                //The robot is on its way to a global objective -> get NEXT_VERTEX from its path:
                i_path++; //desde que nao passe o tamanho do path
        
                if (i_path<elem_s_path){
                    next_vertex=path[i_path];     
                }else{    
                    inpath = false; 
                }
            }
    
            if (!inpath){
                elem_s_path = heuristic_pathfinder_conscientious_cognitive(current_vertex, vertex_web, instantaneous_idleness, dimension, path);
      
/*      printf("Path: ");
      for (i=0;i<elem_s_path;i++){
    if (i==elem_s_path-1){
      printf("%u.\n",path[i]);
    }else{
      printf("%u, ",path[i]);
    }
      }
*/     
            //we have the path and the number of elements in the path
            i_path=1;
            next_vertex = path[i_path];
            inpath = true;
//       printf("Move Robot to Vertex %d (%f,%f)\n", next_vertex, vertex_web[next_vertex].x, vertex_web[next_vertex].y);
        }
    
        /** SEND GOAL (REACHED) AND INTENTION **/
        send_goal_result (current_vertex, next_vertex);
    
        if (inpath){
            //Send the goal to the robot (Global Map)
            ROS_INFO("Sending goal - Vertex %d (%f,%f)\n", next_vertex, vertex_web[next_vertex].x, vertex_web[next_vertex].y);
            sendGoal(ac,vertex_web[next_vertex].x, vertex_web[next_vertex].y);
        }    
    
        goal_complete = false; //garantir q n volta a entrar a seguir aqui
//     printf("ID_ROBOT [3] = %d\n",ID_ROBOT); //-1 in the case there is only 1 robot.
    
    }
    else { //goal not complete (active)
        
        if (interference){          
            do_interference_behavior();     
        }       
        
        if(ResendGoal){
            //Send the goal to the robot (Global Map)
            ROS_INFO("Sending goal - Vertex %d (%f,%f)\n", next_vertex, vertex_web[next_vertex].x, vertex_web[next_vertex].y);
            sendGoal(ac,vertex_web[next_vertex].x, vertex_web[next_vertex].y);  
            ResendGoal = false; //para nao voltar a entrar (envia goal so uma vez)
        }
        
        if (end_simulation){
            return;
        }
        
//      printf("ID_ROBOT [4] = %d\n",ID_ROBOT); //-1 in the case there is only 1 robot.
        
    }
    
    ros::Duration delay = ros::Duration(0.1);
    delay.sleep();

  } // while ros.ok
  
//   return 0; 
}

int main(int argc, char** argv) {
  
    Conscientious_Cognitive_Agent agent;
    agent.init(argc,argv);
    agent.run();

    return 0; 
}

