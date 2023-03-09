import socket

def get_available_ch(server_mac, no_of_ch):
  ch = []
  for port in range(0, 11):
    try:
      s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
      s.bind((server_mac, port))
      if len(ch) == no_of_ch:
          return ch
      s.close()
      ch.append(port)
    except OSError:
      pass
  return None
