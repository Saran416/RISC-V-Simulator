import { useState, useEffect } from 'react'
import './Registers.css'
import { useContext } from 'react'
import { DataContext } from '../context/DataContext.jsx'

const Registers = () => {
    const { regs } = useContext(DataContext);  // Get registers from context
    useEffect(() => {
        console.log(regs);
    })
    return (
        <div className='registers'>
            <div className='table'>
                <div className='row'>
                    <div className='regs-name-title'>Register</div>
                    <div className='regs-value-title'>Value</div>
                </div>
                <div className='tbody'>
                    {Object.entries(regs).map(([reg, value]) => (
                        <div key={reg} className="row">
                            <div className='regs-name'>{reg}</div>
                            <div className='regs-value'>{value}</div>
                        </div>
                    ))}
                </div>
            </div>
        </div>
    );
};

export default Registers;