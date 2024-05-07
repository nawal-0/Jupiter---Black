const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const http = require('http');
const express = require('express');
const socketIo = require('socket.io');
const cors = require('cors');

const app = express();
app.use(cors());

const server = http.createServer(app);
const io = socketIo(server, {
  cors: {
    origin: '*',
  },
});

const portName = '/dev/ttyACM0'; 
const port = new SerialPort({path: portName,  baudRate: 115200 });
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', (data) => {
  //console.log('Data received from serial port:', data);
  io.emit('serialData', data);
});

server.listen(3001, () => {
  console.log('Server is running on port 3001');
});
