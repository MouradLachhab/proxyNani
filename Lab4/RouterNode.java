import javax.swing.*;        

public class RouterNode {
  private int myID;
  private GuiTextArea myGUI;
  private RouterSimulator sim;
  private int[] costs = new int[RouterSimulator.NUM_NODES];
  private String[] route = new String[RouterSimulator.NUM_NODES];
  private int[][] distanceTable = new int[RouterSimulator.NUM_NODES][RouterSimulator.NUM_NODES];
  private int[] neighbours = new int[RouterSimulator.NUM_NODES];
  private Boolean poisonReverse = true;

  private void Update() {
    int temp[] = new int[RouterSimulator.NUM_NODES];
    for(int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(i != myID && distanceTable[myID][i] != RouterSimulator.INFINITY) 
      {
        System.arraycopy(distanceTable[myID], 0, temp, 0, RouterSimulator.NUM_NODES );

        if(poisonReverse){
          for (int j = 0; j < RouterSimulator.NUM_NODES; ++j)
          {
            if (route[j] != "-" && Integer.parseInt(route[j]) == i) {
              temp[j] = RouterSimulator.INFINITY;
            }
          }
        }

        sendUpdate(new RouterPacket(myID, i, temp));
      }
    }
  }

  //--------------------------------------------------
  public RouterNode(int ID, RouterSimulator sim, int[] costs) {
    myID = ID;
    this.sim = sim;
    myGUI =new GuiTextArea("  Output window for Router #"+ ID + "  ");

    System.arraycopy(costs, 0, this.costs, 0, RouterSimulator.NUM_NODES);
    System.arraycopy(costs, 0, this.distanceTable[myID], 0, RouterSimulator.NUM_NODES);
    System.arraycopy(costs, 0, this.neighbours, 0, RouterSimulator.NUM_NODES);
    distanceTable[myID][myID] = 0;

    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(distanceTable[myID][i] != RouterSimulator.INFINITY)
        route[i] = F.format(i,1);
      else
        route[i] = "-";
    }
    printDistanceTable();
    Update();
  }

  //--------------------------------------------------
  public void recvUpdate(RouterPacket pkt) {
      myGUI.println("New update received from router" + pkt.sourceid);
      Boolean changesMade = false;
      int i;

      for(i = 0; i < RouterSimulator.NUM_NODES; ++i) {
        myGUI.println("Cost to " + i + " = " + pkt.mincost[i]);
        if(distanceTable[pkt.sourceid][i] != pkt.mincost[i]){
          distanceTable[pkt.sourceid][i] = pkt.mincost[i];
          changesMade = true;
        }
      }
      if(changesMade) {

        changesMade = false;
        for(i = 0; i < RouterSimulator.NUM_NODES; ++i) 
        {
          if(i != myID) {

            // If they were changes were made to our old route, we need to update the distance
            if(distanceTable[myID][i] != distanceTable[Integer.parseInt(route[i])][i] + distanceTable[myID][Integer.parseInt(route[i])]) {
              distanceTable[myID][i] = distanceTable[Integer.parseInt(route[i])][i] + distanceTable[myID][Integer.parseInt(route[i])];
              changesMade = true;
            }

            if(distanceTable[myID][i] > neighbours[i]) {
              distanceTable[myID][i] = neighbours[i];
              route[i] = F.format(i,1);
              changesMade = true;
            }
            for(int j = 0; j < RouterSimulator.NUM_NODES; ++j) {
              // If going through the Node i is shorter than our current path to J, we change path to go through i.
              if(distanceTable[myID][j] > distanceTable[myID][i] + distanceTable[i][j]) {
                distanceTable[myID][j] = distanceTable[myID][i] + distanceTable[i][j];
                route[j] = F.format(i,1);
                changesMade = true;
              }
            }
          } 
        }
      }
      
        /*if((Integer.parseInt(route[i]) == pkt.sourceid && pkt.mincost[i] != distanceTable[pkt.sourceid][i]) || distanceTable[myID][Integer.parseInt(route[i])] + distanceTable[Integer.parseInt(route[i])][i] > distanceTable[myID][pkt.sourceid] + pkt.mincost[i]) 
        {
          distanceTable[myID][i] = distanceTable[myID][pkt.sourceid] + pkt.mincost[i];
          route[i] = F.format(pkt.sourceid, 1);
          changesMade = true;
        }*/
    
    if(changesMade) {
      Update();
    }
  }
  

  //--------------------------------------------------
  private void sendUpdate(RouterPacket pkt) {
    sim.toLayer2(pkt);
  }
  

  //--------------------------------------------------
  public void printDistanceTable() {
	  myGUI.println("Current table for " + myID +
			"  at time " + sim.getClocktime());
    myGUI.println();
    /*
    myGUI.println("Distancetable:");
    myGUI.print(F.format("  dst |", 12));
    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
     myGUI.print(F.format(String.valueOf(i), 15));
    }
    myGUI.println();
    myGUI.println("-------------------------------------");
    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
      if((i != myID) && (costs[i] != RouterSimulator.INFINITY)) 
      {
        myGUI.print(F.format("nbr " + i + " |", 15));
        myGUI.println();
      }
    }
    myGUI.println();
    */
    myGUI.println("Our distance vector and routes:");
    myGUI.print(F.format("  dst |", 10));
    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
      myGUI.print(F.format(String.valueOf(i), 20));
    }
    myGUI.println();
    myGUI.println("-------------------------------------");

    myGUI.print(F.format("cost |", 13));
    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
      myGUI.print(F.format(String.valueOf(distanceTable[myID][i]), 15));
    }
    myGUI.println();
    myGUI.print(F.format("route |", 10));
    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
     myGUI.print(F.format(String.valueOf(route[i]), 15));
    }
    myGUI.println("\n");
    myGUI.println("--------------------------------------------------------------------------------------------------------------");

  }

  //--------------------------------------------------
  public void updateLinkCost(int dest, int newcost) {
    neighbours[dest] = newcost;

    if(Integer.parseInt(route[dest]) == dest)
      distanceTable[myID][dest] = newcost;
    else if(newcost < distanceTable[myID][dest]) {
      distanceTable[myID][dest] = newcost;
      route[dest] = F.format(dest, 1);
    }
    for(int i = 0; i < RouterSimulator.NUM_NODES; ++i) 
    {
      if(distanceTable[myID][i] > distanceTable[myID][dest] + distanceTable[dest][i]) {
        distanceTable[myID][i] = distanceTable[myID][dest] + distanceTable[dest][i];
        route[i] = F.format(dest, 1);
      }
    }
    Update();
  }
}