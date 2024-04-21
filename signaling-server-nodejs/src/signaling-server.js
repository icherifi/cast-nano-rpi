//ilyas.cherifialaoui@gmail.com

const http = require('http');
const websocket = require('websocket');
const express = require('express');
const app = express();
const path = require('path');

// Serve static files from the "client-js" directory
app.use(express.static(path.join(__dirname, '../../client-js')));

// Start the server
const server = app.listen(8001, () => {
  console.log('Server started on http://localhost:8001');
});

const clients = {};
let receiver = null; // Store the receiver object

const httpServer = http.createServer((req, res) => {
  console.log(`${req.method?.toUpperCase()} ${req.url}`);

  const respond = (code, data, contentType = 'text/plain') => {
    res.writeHead(code, {
      'Content-Type' : contentType,
      'Access-Control-Allow-Origin' : '*',
    });
    res.end(data);
  };

  respond(404, 'Not Found');
});

const wsServer = new websocket.server({httpServer});
wsServer.on('request', (req) => {
  console.log(`WS  ${req.resource}`);

  const {path} = req.resourceURL;
  if (!path) {
    console.error(`Missing path in request`);
    return;
  }
  const splitted = path.split('/');
  splitted.shift();
  const id = splitted[0];

  const conn = req.accept(null, req.origin);

  conn.on('message', (data) => {
    if (data.type === 'utf8') {
      console.log(`Client ${id} << ${data.utf8Data}`);

      const message = JSON.parse(data.utf8Data);
      const destId = message.id;

      if (message.type == "offer"){
        receiver = {id, type: "receiver", offerData: JSON.parse(data.utf8Data)};
        console.log("New receiver");
      }
      const dest = clients[destId]; 
      if (message.type == "answer") {
        if (dest) {
          message.id = id;
          const data = JSON.stringify(message);
          console.log(`Client ${destId} >> ${data}`);
          dest.send(data); //!\ Send answer from to Web client to c++ 
        } else {
          console.error(`Client ${destId} not found`);
        }
      }

      if (message.type == "candidate") {
        if (receiver != null) { //!\ If there is a receiver waiting, get it offer data
          const dest = clients[id];
          data = JSON.stringify(receiver.offerData);
          console.log(`Client ${destId} >> ${data}`);
          dest.send(data); 
        }
      }
    }
  });

  conn.on('close', () => {
    delete clients[id];
    console.error(`Client ${id} disconnected`);
  });

  conn.on('error', (err) => {
    console.error(`Client ${id} error: ${err}`);
  });

  clients[id] = conn;
});

const endpoint = process.env.PORT || '8000';
const splitted = endpoint.split(':');
const port = splitted.pop();
const hostname = splitted.join(':') || '127.0.0.1';

httpServer.listen({port, hostname},
                  () => { console.log(`Server listening on ${hostname}:${port}`); });
