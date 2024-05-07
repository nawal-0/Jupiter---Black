import React, { useEffect, useState } from 'react';
import io from 'socket.io-client';
import ActivityIcon from '../components/activity-icon';
import icons from '../data/icons';


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
        <h1 className='title'>Activity Classification</h1>
        <h3>{data}</h3>
        <div style={{display: 'flex', flexDirection: 'row', justifyContent: 'space-evenly'}}>
            <ActivityIcon image={icons.sitImage} encoding={"0000"} data={data} />
            <ActivityIcon image={icons.standImage} encoding={"LED state: ON"} data={data} />
            <ActivityIcon image={icons.walkImage} encoding={"0010"} data={data} />
            <ActivityIcon image={icons.runImage} encoding={"0011"} data={data} />
        </div>



        </main>
        </div>
    );
}