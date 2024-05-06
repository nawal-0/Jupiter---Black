import React, { useEffect, useState } from 'react';
import io from 'socket.io-client';
import ActivityIcon from '../components/activityIcon';
import icons from '../icons';

const socket = io('http://localhost:3001');

export default function Home() {
    const [data, setData] = useState('hello');
    useEffect(() => {
        socket.on('serialData', (data) => {
            setData(data);
            //console.log('Data received from server:', data);
        });
    }, []);

    return (
        <div>
        <main>
        <h1 className='title'>Home</h1>
        <h2>{data}</h2>
        <ActivityIcon image={icons.standImage} data={data} />
        <ActivityIcon image={icons.runImage} data={data} />



        </main>
        </div>
    );
}