import React from "react";
import styled from 'styled-components';

const Input = styled.input`
    color: #0d143b;
    padding: 1em;
    margin: 0.5em;
    background: #d7e3f9;
    border: none;
    border-radius: 3px;
    font-size: 1em;
`;

function CustomInput({placeholder, type, onChange}) {
    return (
        <Input placeholder={placeholder} type={type} onChange={onChange}/>
    );
}

export default CustomInput;