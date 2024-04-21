from http.server import BaseHTTPRequestHandler, HTTPServer
import subprocess

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length).decode('utf8')

        subprocess.run(["./animation-selector", data])

        self.send_response(200)
        self.end_headers()

server = HTTPServer(('localhost', 8081), HTTPRequestHandler)

print("Started server")
server.serve_forever()

