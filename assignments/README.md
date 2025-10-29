## Questions

1.Write a program to capture all the packets in promiscuous mode.

2. Modify above program to report the upper layer protocol of each captured packet.
(Hints: Use sll protocol field of struct sockaddr ll to report the protocols.
The protocol numbers are found in the linux/if ether.h library file.)

3. Prepare and inject junk Ethernet packets into the network. (*)

4. Write a program like ifconfig to get informations about the interfaces in a computer.

- all programs uses linux APIs, open raw sockets / change interface flags / inject packets