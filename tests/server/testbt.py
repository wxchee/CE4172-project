import socket
from util import get_available_ch

# Replace this with the UUID of your custom GATT service and characteristic
SERVICE_UUID = "180A"
CHARACTERISTIC_UUID = "2A56"
server_bt_mac = '0c:7a:15:ac:75:78'
# Create a Bluetooth socket
server_socket = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)

# Bind the socket to a port
CHANNELS = get_available_ch(server_bt_mac, 2)
print('avalable port', CHANNELS) 

server_socket.bind((server_bt_mac, CHANNELS[0]))

# Listen for incoming connections
server_socket.listen(1)

print("Waiting for connection...")

# Accept a connection
client_socket, client_address = server_socket.accept()
print("Accepted connection from", client_address)

# Create a GATT service and characteristic
service = bytes.fromhex(SERVICE_UUID)
characteristic = bytes.fromhex(CHARACTERISTIC_UUID)
advertise_data = b"\x02\x01\x06\x03\x03\x12\x18" + b"\x16" + service + b"\x0e\x00" + b"\x18" + characteristic

# Advertise the GATT service
client_socket.send(advertise_data)

print("Waiting for data...")

while True:
    # Read data from the client
    data = client_socket.recv(1024)
    if not data:
        break

    # Print the received data
    print("Received data:", data)

# Clean up
print("Closing connection...")
client_socket.close()
server_socket.close()