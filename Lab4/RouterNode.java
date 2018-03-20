import javax.swing.*;        

public class RouterNode {
  private int myID;
  private GuiTextArea myGUI;
  private RouterSimulator sim;
  private int[] costs = new int[RouterSimulator.NUM_NODES];
  private String[] route = new String[RouterSimulator.NUM_NODES];

  //--------------------------------------------------
  public RouterNode(int ID, RouterSimulator sim, int[] costs) {
    System.out.println("New router: " + ID);
    myID = ID;
    this.sim = sim;
    myGUI =new GuiTextArea("  Output window for Router #"+ ID + "  ");

    System.arraycopy(costs, 0, this.costs, 0, RouterSimulator.NUM_NODES);
    costs[myID] = 0;

    for (int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(costs[i] != RouterSimulator.INFINITY)
        route[i] = F.format(i,1);
      else
        route[i] = "-";
    }

    printDistanceTable();
  }

  //--------------------------------------------------
  public void recvUpdate(RouterPacket pkt) {
      System.out.println("FUCKKKAIJABWIDWD");
      myGUI.println("New update received");
      int i;
      for(i = 0; i < RouterSimulator.NUM_NODES; ++i) 
      {
        myGUI.println("Cost to " + i + " = " + pkt.mincost[i]);
        if(costs[pkt.sourceid] + pkt.mincost[i] < costs[i]) 
        {
          costs[pkt.sourceid] = costs[pkt.sourceid] + pkt.mincost[i];
          route[i] = F.format(pkt.sourceid, 1);
        }
      }

    for(i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(i != myID && costs[i] != RouterSimulator.INFINITY) {
          RouterPacket newPkt = new RouterPacket(myID, i, costs);
          sendUpdate(newPkt);
      }
    }
  }
  

  //--------------------------------------------------
  private void sendUpdate(RouterPacket pkt) {
    sim.toLayer2(pkt);

  }
  

  //--------------------------------------------------
  public void printDistanceTable() {
    System.out.println("Try to print");
	  myGUI.println("Current table for " + myID +
			"  at time " + sim.getClocktime());
    myGUI.println();
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
  }

  //--------------------------------------------------
  public void updateLinkCost(int dest, int newcost) {
    costs[dest] = newcost;
    for(int i = 0; i < RouterSimulator.NUM_NODES; ++i)
    {
      if(i != myID && costs[i] != RouterSimulator.INFINITY) {
        RouterPacket newPkt = new RouterPacket(myID, i, costs);
        sendUpdate(newPkt);
      }
    }
  }

}
