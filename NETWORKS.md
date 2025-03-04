# Check out:
Donald Knuth
Bruce Schneier
W. Richard Stevens
The Woz

# Mentioned RFC's:
- Transmission Control Protocol - TCP => RFC 793
- Internet Protocol - IP = > RFC 791
- User Datagram Protocol - UDP => RFC 768

## Socket:
- Speak to other programs using std unix file descriptors
- Raw sockets are very powerful, look it up!

## Socket File Descriptor:
- Integer associated with an open file
- When Unix does I/O, it does so by using file descriptors
- The file can be a network connection, FIFO, pipe, terminal, on-the-disk file, etc.

## Socket Types:
- DARPA internet addresses (Internet Sockets)
- Path names on a local node (Unix sockets)
- CCITT X.25 addresses (X.25 sockets, ignore these!)
- Many other depending on distribution

Type 1.: Stream sockets => SOCK_STREAM
Type 2.: Datagram sockets / connectionless => SOCK_DGRAM

## Stream Socket:
- Two-way connected communication stream
- If you input "1, 2" into the socket, they arrive in order "1, 2"
- They use the "Transmission Control Protocol", also known as TCP)
- TCP sends data sequentially and error-free
- Uses IP for routing, hence TCP/IP
- Ex. uses: characters on ssh, telnet applications, HTTP to get pages

## Datagram Socket:
- If you send a dg, it may arrive, arrive out of order. If it arives the packet is error-free
- Uses UDP
- Uses IP for routing
- Dont have to maintain an open connection as with stream sockets, hence connectionless
- Used when TCP is unavailable or when packet loss is not a problem
- Build packet => Give it IP haeder with destination info => Send
- Ex. uses: tftp (trivial file transfer protocol), dhcpcd (DHCP client), multiplayer games, streaming audio, video conferencing, etc.
- Transfering binary applications mean data cant be lost if we expect the application to work. UDP means we can lose packets, we solve this by adding a protocol on top of UDP. Ex.: tftp for each packet sent, recipient has to end back an "ACK -> "i got it" packet. If this is not present, we send the packet again. This is important to understand when implementing a reliable SOCK_DGRAM app

## Difference between TCP and UDP:
- UDP is fast, fire and forget and its much faster than keeping track of what arrived, making sure its in order, etc.
- TCP is great for sending chat messages
- UDP is great to send 40 positional updates / second of players in the world if its okay to drop 1 or 2

## Data encapsulation:
- New packet => Packet wrapping (encapsulated) in header and rarely a footer by first protocol (TFTP) => then everything (TFTP header included) by UDP => encapsulated by IP => encapsulated by final protocol on the hardware's physical layer, Ethernet
- Another computer strips the ethernet header => kernel strips IP and UDP headers => TFTP program strips TFTP header => we got to the data

## Layered Network Model (ISO/OSI):
- Writing socket programs that are exactly the same without caring how the data is physically transmitted (serial, thin Ethernet, AUI, etc.)

```
****************
* Application  *    => User interaction with the network
* Presentation *    =>
* Session      *    =>
* Transport    *    =>
* Network      *    =>
* Data Link    *    =>
* Physical     *    => Hardware (serial / ethernet)
****************
```

## A more Unix consistent layered model might be:
```
*******************************************
* Application Layer (telnet, ftp)         *
* Host-to-Host Transport Layer (TCP, UDP) *
* Internet Layer (IP and routing)         *
* Network Access Layer (Ethernet, wi-fi)  *
*******************************************
```

For stream sockets we just send() the data out
For datagram sockets we encapsulate the packet in the method of our choosing and sendto() it out
The kernel builds the Transport Layer and Internet Layer on for us
The hardware does the Network Access Layer


## IP addressses:
- Internet Protocol Version 4 - IPv4
- Addresses of four bytes (four octets), ex.: 192.0.2.111. => 2^32 max amount
- Loopback addr: 127.0.0.1 => localhost

IPv6 was born, because we ran out of addresses in IPv4
- 2^128 is max amount for possible addresses
- hexidecimal representation: 2001:0db8:c9d2:aee5:73e3:934a:a5ae:9551

You can condense down IPv6 addresses by leaving off leading zeroes for each byte pair
2001:0db8:c9d2:0012:0000:0000:0000:0051
2001:db8:c9d2:12::51

2001:0db8:ab00:0000:0000:0000:0000:0000
2001:db8:ab00::

0000:0000:0000:0000:0000:0000:0000:0001
::1

::1 is the loopback address => localhost

## Representing an IPv4 address as IPv6:
192.0.2.33 =>::ffff:192.0.2.33

## Subnets:
Network portion, host portion
192.0.2.12 => first 3 bytes are the network, the last byte is the host
OR host 12 on network 192.0.2.0

1 byte for network 3 for host => 24 bits worh of hosts on the network => Class A
3 bytes of network 1 for host => 256 hosts => Class C

Netmask: bitwise-AND with the IP address to get the network number out of it
Ex.: 255.255.255.0
If your IP is 192.0.2.12 AND the netmask is 255.255.255.0 => gives 192.0.2.0
255.255.255.252 which is 30 bits of network, 2 bits of host
The netmask is always a bunch of 1-bits followed by a bunch of 0-bits
New style: 192.0.2.12/30

For IPv6:
2001:db8::/32
2001:db8:5413:4028::9db9/64

## Port numbers:
16-bit number number that represents the local address for the connection
IP address => Street address of a hotel
Port number => Room number
HTTP => 80
SMTP => 25
telnet => 23
Ports under 1024 are often considered special, and usually require special OS privileges to use

## Byte Order:
Representing a two-byte hex number: b34f, we store it in two sequential bytes b3 followed by 4f.
This number stored with the big end first is called Big-Endian
Big-Endian => Network Byte Order (NBO)
Intel stores the bytes reversed so b34f would be stored 4f followed by b3, this is called Little-Endian

Computer stores in Host Byte Order (HBO)
Ex.: Intel 80x86 HBO is Little-Endian, Motorola 68k HBO is Big-Endian, PowerPC HBO is ???
When building packets or filling out data structures we'll need to make sure our two- and four byte numbers are in Network Byte Order. How do we do this when we dont know the native Host Byte order?
We assume HBO isn't right, and we set it to NBO using a function so we can run code on all machines regardless of endians
We convert:
- short => 2 bytes
- long => 4 bytes
Works for unsigned variations as well
Ex.: Converting short from HBO to NBO starts with:
h|ost |to| n|etwork s|hort => h-to-n-s => htons()
h|ost |to| n|etwork s|hort => h-to-n-s => htons()
n|etwork |to| n|etwork s|hort => h-to-n-s => htons()
n|etwork |to| n|etwork s|hort => h-to-n-s => htons()

We want to convert numbers to NBO before they go out on the wire, HBO as they come in

## structs:

### 1. First thing we call when making a connection

```C
struct addrinfo {
    int             ai_flags;       // AI_PASSIVe, AI_CANNONAME
    int             ai_family;      // AF_INET, AF_INET6, AF_UNSPEC
    int             ai_socktype;    // SOCK_STREAM, SOCK_DGRAM
    int             ai_protocol;    // use 0 for "any"
    size_t          ai_addrlen;     // size of ai_addr in bytes
    struct sockaddr *ai_addr;       // struct sockaddr_in or _in6
    char            *ai_canonname;  // full canonical hostname

    struct addrinfo *ai_next;       // linked list, next node
};
```

### 2. We call getaddrinfo()
Returns a ptr to a new linked list of addrinfo's filled out with the data we need
ai_family => define if we want to use IPv4 or IPv6, leave it as AF_UNSPEC to use whatever
ai_next => points to the next element, but it could have several results to choose from, use the first result that worked
ai_addr => ptr to struct sockaddr, contains IP addr strucutre

Usually its fine to just call getaddrinfo() to fill these structs
But we do need to peer inside them to get the values out

```C
struct sockaddr {
    unsigned short      sa_family;      // addr family, AF_xxx
    char                sa_data[14];    // 14 bytes of protocol addr
};
```
sa_family => can be many things but usually AF_INET (IPv4) or AF_INET6 (IPv6)
sa_data => destination address and port for the socket

```C
struct sockaddr_in {                    // this is for IPv4, "in" stands for Internet
    short int               sin_family;     // address family, AF_INET
    unsigned short int      sin_port;       // port number
    struct in_addr          sin_addr;       // internet address
    unsigned char           sin_zero[8];    // same size as struct sockaddr
};
```
sockaddr_in6 for IPv6
sin_family corresponds to sa_family in struct sockaddr and has to be set to AF_INET
sin_port => must be in NBO, hence htons()
sin_zero[8] should be set to all zeros with memset()
sin_addr => explained below

```C
struct in_addr {  // also known as "ina", this is for IPv6 Internet address
    uint32_t  s_addr; // 32-bit int (4 bytes)
};
```
in6_addr for IPv6

```C
struct sockaddr_in6 {   // IPv6
    u_int16_t           sin6_family;   // address family, AF_INET6
    u_int16_t           sin6_port;     // port number, Network Byte Order
    u_int32_t           sin6_flowinfo; // IPv6 flow information
    struct in6_addr     sin6_addr;     // IPv6 address
    u_int32_t           sin6_scope_id; // Scope ID
};
struct in6_addr {   // IPv6
    unsigned char   s6_addr[16];   // IPv6 address
};
```

```C
struct sockaddr_storage {   // large enough to hold both IPv4 and IPv6, sometimes we dont know what it will have
    sa_family_t     ss_family;  // address family

    // implementation specific padding, ignore
    char        __ss_pad1[_SS_PAD1SIZE];
    int64_t     __ss_align;
    char        __ss_pad2[_SS_PAD2SIZE];
};
```
ss_family => check if it has AF_INET or AF_INET6, which we can cast to a sockaddr_in or sockaddr_in6 struct

## IP address manipulation:
inet_pton() => converts IP address in numbers and dots notation into struct in_addr or struct in6_addr, depending on whether AF_INET or AF_INET6 has been specified
pton stands for "printable to network"

```C
struct sockaddr_in  sa;     // IPv4
struct sockaddr_in6 sa6;    // IPv6

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));             // IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6
```
Returns -1 on error, or 0 if the address is messed up
Check to make sure result is greater than 0 before using!!!

Other way around
inet_ntop() => converts struct in_addr to numbers and dots or hex and colons
ntop stands for "network to printable"

```C
// IPv4
char ip4[INET_ADDRSTRLEN]   // space to hold IPv4 string
struct sockaddr_in sa;      // data

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
printf("The IPv4 address is: %s\n", ipv4);


// IPv6
char ip6[INET6_ADDRSTRLEN]  // space to hold IPv6 string
struct sockaddr_in6 sa6     // data

inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
printf("The address is: %s\n", ip6);
```

















## Mentioned Functions:
send() => send data over TCP socket
recv() => receive data over TCP socket
socket() => returns socket descriptor which we use for communication
htonl() => convert values betwen host and network byte order
connect() =>
getaddrinfo() =>
bind() => fills out own struct sockaddr_in to connect()

sendto() => send data over UDP socket
recvfrom() => receive data over UDP socket, SOCK_DGRAM

## Function descriptions:
send(int s, const void *buf, size_t len, int flags) => for SOCK_STREAM sockets
s => socket
buf => data ptr we want to send
len => num of bytes we are sending
flags => specify how data is sent, set to 0 if normal data (most common: MSG_OOB, MSG_DONTROUTE, MSG_DONTWAIT, MSG_NOSIGNAL)
RETURN => number of bytes actually sent or -1 on error (errno set accordingly)
- num of bytes may be less than the number we asked to send, refer "handling partial send()s"
- if socket closed by either side, send() will get SIGPIPE

recv(int s, void *buf, size_t len, int flags) => for SOCK_STREAM sockets
s => socket descriptor
the rest are the same as send()
(most common flags: MSG_OOB, MSG_PEEK, MSG_WAITALL)
- blocks until there is data to read
- if we dont want block, set the socket to non-blocking or check with select() or poll() to see if there is incomming data before calling recv()
RETURN => number of bytes actually received or -1 on error (errno set accordingly)
- num of bytes may be less than specific in len param
- if remote side closed connection, recv() returns 0, this is good for determining if remote side has closed the connection

connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
sockfd => socket file descriptor
serv_addr => contains destination port and ip address
addrlen => len in bytes of the server address structure
- use getaddrinfo() to fetch the info above
- bind() fills out sockaddr_in to pass connect()
- check return value, returns -1 on error (sets errno accordingly)
- we dont need to call bind(), we dont care for the local port, we only care about the remote port, kernel choses local port, the site will automatically get us the information
