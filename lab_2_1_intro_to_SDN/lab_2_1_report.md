# Lab 1: Introduction to SDN

## General Instructions

In this series of labs, we will look at more advanced concepts in computer networks. We will focus on Software Defined Networks (SDN),a new paradigm in network management used nowadays in many large-scale deployments.

The successful completion of the labs requires your provision of a report for each lab. All exercises in the instructions must be answered unless stated otherwise. Your answers should be concise; you don’t need more than a few lines to answer each question. Questions that need to be answered are within boxes. Some exercises ask you to discuss with your lab partner. You do not need to provide written answers to those questions.

You should complete the labs in groups of two people — use the group you’ve created in Canvas! You are of course encouraged to discuss with other groups, but all your submissions must be the results of your own work. Once finished, upload your solution as a PDF document to Canvas, and don’t forget to identify both members of the group.

Additional documents, such as source code, are available on Canvas. 

It is assumed that you run the labs in the windows environment at Chalmers. We use a virtual machine and VirtualBox to have access to a Linux environment. You may use your own computers; however, we might not be able to provide support in that case. If you haven’t done it yet, please watch the videos on SDN by David Mahler. A link is given on Canvas

## Software Defined Network

In conventional networks, the control and management are done locally by dedicated equipment, such as routers and switches. Special algorithms, rule sets and specific hardware (ASICs) are used to perform operations like packet routing and flow forwarding. Configuration is device-centric, and topology changes require human intervention or long transition phases to come back to normal performance. The lack of flexibility can especially hinder performances in high traffic networks such as Internet Service Providers backbones and within datacenters.

In SDN, the control and data planes are decoupled. The forwarding logic can now be programmed and is not integrated into the hardware as before. The control plane can therefore fully observe the system and sends commands to the elements of the data plane to configure them.

*The control plane is composed of one or more controllers. A controller centrally defines the behavior of the network, by computing forwarding rules on request. Switches compose most of the elements of the data plane (along with routers) and are called learning switches.*

## Setup

We will experiment with Software Defined Networks using **Mininet**. Mininet is a network emulator that can emulate switches, controllers and execute application code. It is available for many Linux distributions. If you are already using Linux, you can install it directly, although we recommend you use a virtual machine (VM). Mininet makes updates to your network interfaces to work and might affect your system.

We will use VirtualBox to run our virtual machine. VirtualBox is available for Windows, Mac OS, and Linux. You can use another hypervisor of your choice but be aware that we will only support you for technical problems if you use VirtualBox.

## Testing your environment

## Part 1 - Network Discovery

In the floodlight directory and starts your controller with the command: 

`java −jar target/ floodlight.jar`

Once it is ready (you should see *Sending LLDP packets out of all the enabled ports* printed regularly), 

open a second terminal and start the custom network topology we gave you, with the command:

`sudo python topo lab1.py`

If everything is working correctly, you should see that the controller registered a switch.

#### Question 1. In a conventional network, how can a switch know where to forward an incoming packet? Please briefly explain the mechanism.

In a conventional network, a switch determines where to forward an incoming packet using a MAC address table. When a new frame arrives, the switch checks the destination MAC address in the table: If a match is found, the switch forwards the frame to the corresponding port. If not, it floods the frame to all ports except the one it came from.

#### Ping from one host to another. You should see that at the very beginning, the RTT is significantly longer. Why? Stop pinging and wait for about 30 seconds and try again. Is the RTT now stable? Why?
‘mininet> h1 ping h2
PING 10.0.0.2 (10.0.0.2) 56(84) bytes of data.
64 bytes from 10.0.0.2: icmp_seq=1 ttl=64 time=110 ms
64 bytes from 10.0.0.2: icmp_seq=2 ttl=64 time=0.166 ms
64 bytes from 10.0.0.2: icmp_seq=3 ttl=64 time=0.034 ms
64 bytes from 10.0.0.2: icmp_seq=4 ttl=64 time=0.040 ms
64 bytes from 10.0.0.2: icmp_seq=5 ttl=64 time=0.034 ms
64 bytes from 10.0.0.2: icmp_seq=6 ttl=64 time=0.034 ms
64 bytes from 10.0.0.2: icmp_seq=7 ttl=64 time=0.035 ms
^C
--- 10.0.0.2 ping statistics ---
7 packets transmitted, 7 received, 0% packet loss, time 6107ms
rtt min/avg/max/mdev = 0.034/15.788/110.179/38.534 ms
mininet> h2 ping h1
PING 10.0.0.1 (10.0.0.1) 56(84) bytes of data.
64 bytes from 10.0.0.1: icmp_seq=1 ttl=64 time=3.68 ms
64 bytes from 10.0.0.1: icmp_seq=2 ttl=64 time=0.034 ms
64 bytes from 10.0.0.1: icmp_seq=3 ttl=64 time=0.028 ms
64 bytes from 10.0.0.1: icmp_seq=4 ttl=64 time=0.029 ms
64 bytes from 10.0.0.1: icmp_seq=5 ttl=64 time=0.031 ms
64 bytes from 10.0.0.1: icmp_seq=6 ttl=64 time=0.033 ms
64 bytes from 10.0.0.1: icmp_seq=7 ttl=64 time=0.031 ms
^C
--- 10.0.0.1 ping statistics ---
7 packets transmitted, 7 received, 0% packet loss, time 6111ms
rtt min/avg/max/mdev = 0.028/0.552/3.683/1.278 ms’

The first RTT is high because the switch does not know the destination and needs to flood the packet; afterwards, the RTT becomes stable because the MAC address table has been established, allowing packets to be forwarded directly.

#### Question 3. Plot the Round-Trip Time (RTT) evolution over time. You need to produce a plot with the runtime as the x-axis and the RTT time as the y-axis. You need two curves, the RTT from h1 to h2, and the RTT from h2 to h1. measurement is not enough! You will need to restart both the network and the controller many times and average over multiple measures.

`mininet> h1 ping -c 10 h2`

`mininet> h2 ping -c 10 h1`

## Part 2 - Understanding OpenFlow

We will now directly look into the memory of the switch. Open vSwitch, the software used to run virtual switches, offers a command to look into the details of OpenFlow, ovs-ofctl.As with Wireshark, you can see the communications between your switch and your controller. Run

`sudo ovs−ofctl snoop s1 −O OpenFlow13`

You can also just check the switch’s state with

`sudo ovs−ofctl show s1 −O OpenFlow13`

`sdn@sdn-eda387:~$ sudo ovs-ofctl show s1 -O OpenFlow13
OFPT_FEATURES_REPLY (OF1.3) (xid=0x2): dpid:0000000000000001
n_tables:254, n_buffers:256
capabilities: FLOW_STATS TABLE_STATS PORT_STATS GROUP_STATS QUEUE_STATS
OFPST_PORT_DESC reply (OF1.3) (xid=0x3):
 1(s1-eth1): addr:6e:42:41:b7:95:4e
     config:     0
     state:      0
     current:    10GB-FD COPPER
     speed: 10000 Mbps now, 0 Mbps max
 2(s1-eth2): addr:1a:0d:66:1a:ce:a2
     config:     0
     state:      0
     current:    10GB-FD COPPER
     speed: 10000 Mbps now, 0 Mbps max
 LOCAL(s1): addr:52:97:4f:6d:cf:49
     config:     0
     state:      0
     speed: 0 Mbps now, 0 Mbps max
OFPT_GET_CONFIG_REPLY (OF1.3) (xid=0x5): frags=normal miss_send_len=0`


#### Question 4. Explain the roles of the different packets you see.

We will now directly look into the memory of the switch. Open vSwitch, the software used 

#### Question 5. Explain what each flow represents. Briefly explain what each field is used for.

`sdn@sdn-eda387:~$ sudo ovs-ofctl dump-flows s1 -O OpenFlow13
OFPST_FLOW reply (OF1.3) (xid=0x2):
 cookie=0x20000000000000, duration=30.038s, table=0, n_packets=29, n_bytes=2842, idle_timeout=5, priority=1,ip,in_port=1,dl_src=1a:38:b1:6f:68:14,dl_dst=7a:d0:4e:ed:b4:b3,nw_src=10.0.0.1,nw_dst=10.0.0.2 actions=output:2
 cookie=0x20000000000000, duration=30.034s, table=0, n_packets=29, n_bytes=2842, idle_timeout=5, priority=1,ip,in_port=2,dl_src=7a:d0:4e:ed:b4:b3,dl_dst=1a:38:b1:6f:68:14,nw_src=10.0.0.2,nw_dst=10.0.0.1 actions=output:1
 cookie=0x0, duration=330.733s, table=0, n_packets=25, n_bytes=1942, priority=0 actions=CONTROLLER:65535`

 - Flow 1: Forward packets from port 1 (from 10.0.0.1 to 10.0.0.2) to port 2

 - Flow 2: Forward packets from port 2 (from 10.0.0.2 to 10.0.0.1) to port 1.

 - Flow 3: Default flow (priority=0), match unmatched packets and send them to the controller.

 `cookie`: used to identify the flow for easier management

 `duration`: how long the flow has existed

 `table`: the table in which the flow resides (here, table 0)

 `n_packets / n_bytes`: number of packets and total bytes that matched this flow

 `idle_timeout`: f no packets match the flow within 5 seconds, the flow is removed

 `priority`: match priority (higher numbers indicate higher priority)

 `ip`: matches IP packets

 `in_port`: matches the input port

 `dl_src / dl_dst`: matches Ethernet source and destination MAC addresses

 `nw_src / nw_dst`: matches IP source and destination addresses

 `actions=output:2`: action to forward the packet to port 2

 #### Question 6. Imagine now that a web server is running on h1. h2 starts an HTTP request to h1. What will happen? Describe briefly the messages exchanged between the switch and the controller, and the flows that would appear in the dump-flows result. Note: you can try to emulate the behavior by opening a terminal on h2 and by using the wget command and python -m SimpleHTTPServer

 For h1:
`python3 -m http.server 80`

For h2:
`wget http://10.0.0.1`

`sdn@sdn-eda387:~$ sudo ovs-ofctl dump-flows s1 -O OpenFlow13
OFPST_FLOW reply (OF1.3) (xid=0x2):
 cookie=0x20000000000000, duration=2.987s, table=0, n_packets=4, n_bytes=399, idle_timeout=5, priority=1,tcp,in_port=2,dl_src=02:25:8f:7e:c2:21,dl_dst=a2:60:38:2e:fc:db,nw_src=10.0.0.2,nw_dst=10.0.0.1,tp_src=34536,tp_dst=80 actions=output:1
 cookie=0x20000000000000, duration=2.986s, table=0, n_packets=4, n_bytes=842, idle_timeout=5, priority=1,tcp,in_port=1,dl_src=a2:60:38:2e:fc:db,dl_dst=02:25:8f:7e:c2:21,nw_src=10.0.0.1,nw_dst=10.0.0.2,tp_src=80,tp_dst=34536 actions=output:2
 cookie=0x0, duration=329.936s, table=0, n_packets=31, n_bytes=2622, priority=0 actions=CONTROLLER:65535`

 Interpretation of these entries
- First entry: matches TCP packets coming in_port=2 with Ethernet src 02:25:... (h2), dst a2:60:... (h1), IP 10.0.0.2→10.0.0.1, TCP tp_src=34536, tp_dst=80. Action: output:1 (forward to port 1, where h1 is attached). This is the client→server flow.
- Second entry: the reverse flow: matches packets in_port=1 from h1→h2 (IP 10.0.0.1→10.0.0.2, TCP 80→34536) and forwards to output:2. This is server→client.
- Both installed flows have priority=1, idle_timeout=5 and nonzero cookie (controller-installed).
- Third entry: default low-priority rule (priority=0) with action CONTROLLER:65535 — unmatched packets are sent to the controller (this is why the first packet triggered a Packet-In).

####  Floodlight’s User Interface

We have seen how SDN works from the user perspective, and from the switch perspective. We will now investigate how the controller views the network.

Floodlight has two interfaces we can use: an API, and a user interface. While both the network and the controller are running, connect to http://localhost:8080/ui/index. html.

You should see a dashboard appearing, with the switches and the hosts discovered by the controller. Look at the topology and discuss with your partner the different elements. 

- Controller Status

- Switches

- Hosts

Go then to the Switches tab and select the switch. Start pinging again. In the Flows table, do you see the same flows as before? Discuss with your partner. 

[picture]


Stop the network and the controller. We will now try a different topology. Start again the controller, and start a new network with the command: 

`sudo mn --topo=tree,depth=3 --controller=remote,ip=0.0.0.0,port=6653`

#### Question 7. How many switches are present? What kind of topology is it? How many switches must be programmed to ping from h1 to h8? 

- 7

- Tree

- 5






















