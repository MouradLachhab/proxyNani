import javax.swing.*;        

public class RouterNode {
  private int myID;
  private GuiTextArea myGUI;
  private RouterSimulator sim;
  private int[] costs = new int[RouterSimulator.NUM_NODES];
  private String[] route = new String[RouterSimulator.NUM_NODES];
  private int[] neighbours = new int[RouterSimulator.NUM_NODES];

  private void Update() {
    int temp[] = new int[RouterSimulator.NUM_NODES];
    for(int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(i != myID && costs[i] != RouterSimulator.INFINITY) 
      {
        System.arraycopy( costs, 0, temp, 0, costs.length );
        int j;
        for (j = 0; j < RouterSimulator.NUM_NODES; ++j)
        {
          if (route[j] != "-" && Integer.parseInt(route[j]) == i)
            temp[j] = RouterSimulator.INFINITY;
        }
        temp[i] = RouterSimulator.INFINITY;
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
    System.arraycopy(costs, 0, this.neighbours, 0, RouterSimulator.NUM_NODES);
    costs[myID] = 0;

    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(costs[i] != RouterSimulator.INFINITY)
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
      for(i = 0; i < RouterSimulator.NUM_NODES; ++i) 
      {
        myGUI.println("Cost to " + i + " = " + pkt.mincost[i]);
        if(neighbours[pkt.sourceid] + pkt.mincost[i] < costs[i] || (Integer.parseInt(route[i]) == pkt.sourceid && neighbours[pkt.sourceid] + pkt.mincost[i] != costs[i])) 
        {
          costs[i] = neighbours[pkt.sourceid] + pkt.mincost[i];
          route[i] = F.format(pkt.sourceid, 1);
          changesMade = true;
        }
      }

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
      myGUI.print(F.format(String.valueOf(costs[i]), 15));
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
    costs[dest] = newcost;
    neighbours[dest] = newcost;
    route[dest] = F.format(dest,1);
    Update();
  }
}