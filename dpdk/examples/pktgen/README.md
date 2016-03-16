
This extended verion of Pktgen-DPDK supports latency, jitter measurements, timestamping packets and other functionalities. 

The installation process stays the same as the original Pktgen-DPDK. Please refer to https://github.com/pktgen/Pktgen-DPDK 

In this verion of Pktgen-DPDK, you will see latency, jitter, timestamps in the console. An example test setup would be that the packet generator machine connects to another machine with DPDK l2fwd or l3fwd applications. The traffic generator sends the packets to l2fwd/l3fwd which then send the packets back to the packet generator. The end-to-end latency illustrated as t2-t1 and jitter (IPDV) will be measured and shown in the console. 

 |-----t1|-------------->|-----------|

 |-pktgen|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|l2fwd/l3fwd|

 |-----t2|<--------------|-----------|




