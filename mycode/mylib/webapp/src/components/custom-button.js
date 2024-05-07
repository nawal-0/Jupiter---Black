import React, { useEffect, useState } from "react";
import styled from 'styled-components';

const Button = styled.button`
    background-color: #0d273b;
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 2px;
    cursor: pointer;
    border-radius: 8px;
    &:hover {
        background-color: #1f5783;
    }
`;

function CustomButton({text}) {

    const onClick = () => {
        console.log('Button clicked');
    }

    return (
        <Button onClick={onClick}>{text}</Button>
    );
}

export default CustomButton;