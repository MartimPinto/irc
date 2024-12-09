# ft_irc
>A IRC server in C++98

</p>
<p align="center">
	<a href="#about">About</a> •
	<a href="#usage">Usage</a> •
   <a href="#implementation">Implementation</a>
</p>


## About
The ft_irc project consists of implementing a simple **Internet Relay Chat (IRC)** server in C++. The server should be able to accept connections from multiple concurrent clients, handle disconnections, support private messaging and the creation of channels. A client can connect to the server using either **Hexchat** or the `nc` command. The group for this project consisted of myself, [filipepinholiveira](https://github.com/filipepinholiveira) and [diocode](https://github.com/diocode)

<img src="demo.gif" width="100%"/>

## Usage
`git clone` this project and`cd` into its folder. Open a terminal and after you run `make` the program is ready to be executed as so:

```shell
./ircserv <port number> <password>
```

The server should now be ready and listening for connections. There are two ways to connect to the server: the `nc` command and the **Hexchat** IRC client application.

If connecting with `nc` the parameters are:

```shell
nc localhost <port number>
```

Once connected to the server, users have access to these commands:

- PASS:
   - Before any other commands can be sent the user must first be authenticated by using the PASS command and typing the password defined on server creation.
   - **Usage**: `PASS <password> `

- NICK:
   - After authentification the user then must register with a Nickname and a Username. The NICK command allows the user to set a nickname or change to a new one if one is already set.
   - **Usage:** `NICK <nickname>`

- USER:
   - The USER command is used to set the username and realname of the client. The realname parameter must be prefixed by a colon(:). After the Nickname and Username are set then the client is registered and free to use other commands.
   - **Usage:** `USER <username> 0 * <realname>`

- JOIN:
   - JOIN allows users to join a channel or, if that channel does not exist, create one. A user that creates a channel becomes the **operator** of that channel and has access to **operator** exclusive commands. If the channel requires a key then it should also be included.
   - **Usage:** `JOIN <#channel> [key]`

- PRIVMSG:
   - PRIVMSG is the command that allows users to send messages to each other and to channels. A message send to a channel will be received by every member of that channel. It is possible to send the same message to multiple users and channels at the same time.
   - **Usage:** `PRIVMSG <target>{,<target>} <text to be sent>`
     
- PART:
   - The PART command allows a user to leave a channel. If a channel becomes empty after a user leaves then that channel is removed from the list of channels in the Server.
   - **Usage:** `PART <#channel>`
     
- QUIT:
   - When using the QUIT command that user will disconnect from the Server in clean fashion, parting from any channels they are part of and closing the socket they use for communication.
   - **Usage:** `QUIT`
 <br>
 <br>
 
While those commands are available to any user, there is also a list of **operator-exclusive** commands:

- KICK:
   - Operators can use KICK to forcefully remove a user from the channel. If no reason is specified, the server will send a default message.
   - **Usage**: `KICK <#channel> <user> [<comment>]`

- INVITE:
   - The INVITE command is used to invite a user to a channel. If the channel is in Invite Only Mode, only users that are on that channels invite list are able to join. 
   - **Usage:** `INVITE <nickname> <channel>`

- TOPIC:
   - The TOPIC command is used to change or view the topic of the given channel. If the `<topic>` parameter is not given then a message is returned specifying the topic or the lack thereof. If a channel's topic is set, users will be notified of the topic when joining the channel.
   - **Usage:** `TOPIC <#channel> [<topic>]`

- MODE:
   - The MODE command is used to set or remove options (or modes) from a given channel. Modes are set with **+** and removed with **-**. The following modes were implemented:
	   - i: Set/Remove Invite-only channel
	   - t: Set/Remove the restrictions of the TOPIC command to channel operators.
	   - k: Set/Remove the channel key (password)
	   - o: Give/take channel operator privilege
	   - l: Set/remove the user limit to channel
   - **Usage:** `MODE <#channel> <modestring>[<arguments>]`
  
  
When connecting through **Hexchat**, the PASS, NICK and USER commands are called automatically, while in `nc` they need to be passed one by one.

## Implementation
The project is written using the C++98 standard. Network communication is handled by **POSIX Sockets** the process is as follows:
- The server is initialized with a socket bound to a specific port and a IP address. It is then set up using socket(), bind(), and listen() system calls. The server socket is set to non-blocking mode using fcntl(fd, F_SETFL, O_NONBLOCK); to handle multiple connections without stalling.
- The server uses the poll() system call to monitor multiple file descriptors for incoming data or connections. It iterates over the list of file descriptors monotoring for any events: if it is an event on the server socket then it indicates a new connection(`accept()`), but if it is in a client socket then it indicates incoming data (`recv()`).
- When a new client connects, the server accepts the new connection using `accept()`, adds the new client file descriptor to the list and creates a Client object to store all its metadata.
- When a client sneds data, the server reads it using `recv()`, sends it to a buffer to handle partial commands, parses that buffer, executes the command an sends responses back to the client using `send()`


