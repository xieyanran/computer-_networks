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

#### Question 4. Explain the roles of the different packets you see.

We will now directly look into the memory of the switch. Open vSwitch, the software used 

#### Question 5. Explain what each flow represents. Briefly explain what each field is used for.











