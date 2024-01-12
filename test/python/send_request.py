import config
import SocketServer
from http.client import HTTPResponse

def send_request(request_header):
    client = SocketServer.SocketServer(SocketServer.AF_INET, SocketServer.SOCK_STREAM)
    client.connect((config.SERVER_ADDR, config.SERVER_PORT))
    client.send(request_header.encode())
    # read and parse http response
    http_response = HTTPResponse(client)
    http_response.begin()
    return http_response

request_header = 'GET  /  HTTP/1.1\r\nHost:{}\r\n\r\n'.format(config.SERVER_ADDR)
http_response = send_request(request_header)

print(request_header)
print(http_response.status)