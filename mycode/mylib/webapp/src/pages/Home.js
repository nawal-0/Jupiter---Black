import React, { useEffect } from 'react';
//const SerialPort = require('serialport');


export default function Home() {
    // useEffect(() => {
    //     const port = new SerialPort('/dev/ttyACM0', {
    //         baudRate: 9600
    //     });
    //     port.on('data', (data) => {
    //         console.log('Data:', data.toString());
    //     });

    //     return () => {
    //         port.close();
    //     };
    // }, []);

    return (
        <div>
        <main>
        <h1 className='title'>Home</h1>
        </main>
        </div>
    );
}