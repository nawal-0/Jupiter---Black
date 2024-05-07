import React, { useEffect, useState } from "react";

function ActivityIcon({image, encoding, data}) {
    const [opacity, setOpacity] = useState(0.3);

    useEffect(() => {
        if (data.trim() == encoding) {
            setOpacity(1);
        } else {
            setOpacity(0.3);
        }
    }, [data]);

    const iconStyle = {
        opacity: opacity,
        borderRadius: '30px',
        height: '350px',
        width: 'auto',
        padding: '10px',
    };
    return (
        <div>
            <img src={image} alt="activity icon" style={iconStyle}/>
          
        </div>
    );
}

export default ActivityIcon;