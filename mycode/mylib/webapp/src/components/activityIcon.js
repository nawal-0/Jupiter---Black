import React, { useEffect, useState } from "react";

function ActivityIcon({image, encoding, data}) {
    const [opacity, setOpacity] = useState(0.5);

    useEffect(() => {
        if (data.trim() == encoding) {
            setOpacity(1);
        } else {
            setOpacity(0.3);
        }
    }, [data]);

    const imStyle = {
        opacity: opacity,
        borderRadius: '20px',
        height: '450px',
        width: 'auto',
    };
    return (
        <div>
            <img src={image} alt="activity icon" style={imStyle}/>
          
        </div>
    );
}

export default ActivityIcon;