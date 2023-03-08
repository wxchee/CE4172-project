import socket
import select

server_socks = []

CHANNELS = []
CONN_TIMEOUT = 10

# find available port
for port in range(0, 11):
  try:
    s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
    s.bind(("0c:7a:15:ac:75:78", port))
    if len(CHANNELS) == 2:
        break
    s.close()
    CHANNELS.append(port)
  except OSError:
    pass

print('avalable port', CHANNELS) 

# create two server sockets to listen to channel 1 and 2 respectively
for ch in CHANNELS:
  server_sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
  server_sock.bind(("0c:7a:15:ac:75:78", ch))
  server_sock.listen(ch)
  server_socks.append(server_sock)


client_socks = []
for c in [("6e:96:38:55:ab:89", CHANNELS[0]), ("48:c6:2c:eb:f6:f8", CHANNELS[1])]:
  client_sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
  client_sock.settimeout(10.0)
  try:
    client_sock.connect(c)
  except socket.timeout:
    print("Connection timeout for ", c)
  finally:
    client_socks.append(client_sock)


while True:
    readable, writable, exceptional = select.select(server_socks + client_socks)

    for s in readable:
        if s in server_socks:
            client, cAddr = s.accept()
            print("Accept connection from ", cAddr)
        elif s in client_socks:
            d = s.recv(1024)
            if len(d) > 0:
                print("Received: ", d)
