import React from "react";
import styled from 'styled-components';

const Button = styled.button`
    background-color: #0d273b;
    border: none;
    color: white;
    padding: 25px 72px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 10px;
    cursor: pointer;
    border-radius: 8px;
    &:hover {
        background-color: #1f5783;
    }
`;

function CustomButton({text, socket}) {

    const onClick = () => {
        //console.log('Button clicked');
        if (text === 'Start') {
            socket.emit('buttonClicked', 'dev_power on\n');
        } else {
            socket.emit('buttonClicked', 'dev_power off\n');
        }
    }

    return (
        <Button onClick={onClick}>{text}</Button>
    );
}

export default CustomButton;