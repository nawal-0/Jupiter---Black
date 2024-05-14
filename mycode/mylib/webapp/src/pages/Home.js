import React, { useEffect, useState } from 'react';
import Divider from '@mui/material/Divider';
import io from 'socket.io-client';
import ActivityIcon from '../components/activity-icon';
import CustomButton from '../components/custom-button';
import CustomInput from '../components/custom-entry';
import icons from '../data/icons';

//const terms = ['sit', 'stand', 'walk', 'run', ];
const socket = io('http://localhost:3001');

export default function Home() {
    const [data, setData] = useState('hello');
    const [weight, setWeight] = useState('0');
    const [energy, setEnergy] = useState('0');


    useEffect(() => {
        socket.on('serialData', (data) => {
            try {
                // let stringData = '{state: "0001", energy: "50"}';
                // let match = stringData.match(/\{.*\}/);
                let match = data.match(/\{.*\}/);
                console.log(match[0]);
                let parsedData = JSON.parse(match[0]);
                
                if (parsedData.energy != null) {
                    setEnergy(parsedData.energy);
                }
                if (parsedData.state != null) {
                    let state = parsedData.state;
                    if (state === 'sit') {
                        setData('Sitting');
                    } else if (state === 'stand') {
                        setData('Standing');
                    } else if (state === 'walk') {
                        setData('Walking');
                    } else if (state === 'run') {
                        setData('Running');
                    }
                }
            } catch (e) {
                console.log('Error parsing JSON:', e);

            }
            //setData(data);
            //console.log('Data received from server:', data);
        });
    }, []);

    const onEntryChange = (e) => {
        setWeight(e.target.value);
        socket.emit('buttonClicked', "send_weight " + e.target.value + '\n');
    }

    return (
        <div>
        
        <div className='main-container'>
            <div className='left-container'>
                <h1 className='title'>Activity Classification</h1>
                <h3>{data}</h3>
                <div className='icon-container'>
                    <ActivityIcon image={icons.sitImage} encoding={"Sitting"} data={data} />
                    <ActivityIcon image={icons.standImage} encoding={"Standing"} data={data} />
                    <ActivityIcon image={icons.walkImage} encoding={"Walking"} data={data} />
                    <ActivityIcon image={icons.runImage} encoding={"Running"} data={data} />
                </div>
            </div>
            <Divider orientation='vertical' flexItem />
            <div className='right-container'>
                <div className='button-container'>
                    <CustomButton text="Start" socket={socket}/>
                    <CustomButton text="Stop" socket={socket} />
                </div>
                <Divider flexItem />
                <div className='info-container'>
                    <h2>Activity Information</h2>

                    <CustomInput 
                        placeholder="Enter weight (kg)" 
                        type="text" 
                        onChange={onEntryChange} />
                
                    <h3>Weight: {weight}</h3>
                    <h3>Energy Used: {energy}</h3>
                </div>
            </div>

        </div>
        
        </div>
    );
}