import config
import requests

# import logging

# # These two lines enable debugging at httplib level (requests->urllib3->http.client)
# # You will see the REQUEST, including HEADERS and DATA, and RESPONSE with HEADERS but without DATA.
# # The only thing missing will be the response.body which is not logged.
# try:
#     import http.client as http_client
# except ImportError:
#     # Python 2
#     import httplib as http_client
# http_client.HTTPConnection.debuglevel = 1

# # You must initialize logging, otherwise you'll not see debug output.
# logging.basicConfig()
# logging.getLogger().setLevel(logging.DEBUG)
# requests_log = logging.getLogger("requests.packages.urllib3")
# requests_log.setLevel(logging.DEBUG)
# requests_log.propagate = True


def	print_response(r):
    print("Status code : ", r.status_code, "\n")
    print("Response headers")
    print(r.headers, "\n")
    print("Response body")
    print(r.text, "\n")
    return (r.status_code)

def print_request(r):
    print("Request url")
    print(r.request.url, "\n")
    print("Request method")
    print(r.request.method, "\n")
    print("Request headers")
    print(r.request.headers, "\n")
    print("Request body")
    print(r.request.body, "\n")


def	try_get(url, headers, expected):
    res = requests.get(url, headers=headers)
    if (expected != res.status_code):
        print(f"Wrong status code, got {res.status_code}, expected {expected}")
        return (1)
    print_response(res)
        
	
def	try_post(url, headers, body, expected):
    res = requests.post(url, headers=headers, data=body)
    if (expected != res.status_code):
        print(f"Wrong status code, got {res.status_code}, expected {expected}")
        return (1)
    print_response(res)

base_url = "http://" + config.SERVER_ADDR + ":" + config.SERVER_PORT

# try_get(base_url, None, 200)

# GET request
# url = base_url
# res = requests.get(url)
# print_request(res)
# print_response(res)

# # GET index
# url = base_url + '/index.html'
# try_get(url, None, 200)

# url = base_url + '/wrong.html'
# try_get(url, None, 404)
# res = requests.get(url)
# print_request(res)
# print_response(res)

# # GET wit hcustom header
# req_headers = {'user-agent':'custom python script', 'Accept-Language': 'fr'}
# url = base_url + '/index.html'
# res = requests.get(url, headers=req_headers)
# print_request(res)
# print_response(res)

# # GET wit hcustom header
# req_headers = {'host':'none', 'Accept-Language': 'fr'}
# url = base_url + '/index.html'
# res = requests.get(url, headers=req_headers)
# print_request(res)
# print_response(res)



# # HEAD request
# res = rq.head(url)


post_data = {'key1':'value1', 'key2':'vallu2'}
url = base_url + '/wrong.html'
try_post(url, None, post_data, 404)
# # POST request
# post ok 
# post pas ok -> mauvais arg / mauvais endroit

