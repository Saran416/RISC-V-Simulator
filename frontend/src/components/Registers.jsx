import { useState, useEffect } from 'react'
import './Registers.css'
// import { useContext } from 'react'
// import { DataContext } from '../context/DataContext.jsx'

const Registers = () => {
    // const { data } = useContext(DataContext);
    const [regs, setRegs] = useState(
        [   {'name': 'x0', 'value': '0'},
            {'name': 'x1', 'value': '0'},
            {'name': 'x2', 'value': '0'},
            {'name': 'x3', 'value': '0'},
            {'name': 'x4', 'value': '0'},
            {'name': 'x5', 'value': '0'},
            {'name': 'x6', 'value': '0'},
            {'name': 'x7', 'value': '0'},
            {'name': 'x8', 'value': '0'},
            {'name': 'x9', 'value': '0'},
            {'name': 'x10', 'value': '0'},
            {'name': 'x11', 'value': '0'},
            {'name': 'x12', 'value': '0'},
            {'name': 'x13', 'value': '0'},
            {'name': 'x14', 'value': '0'},
            {'name': 'x15', 'value': '0'},
            {'name': 'x16', 'value': '0'},
            {'name': 'x17', 'value': '0'},
            {'name': 'x18', 'value': '0'},
            {'name': 'x19', 'value': '0'},
            {'name': 'x20', 'value': '0'},
            {'name': 'x21', 'value': '0'},
            {'name': 'x22', 'value': '0'},
            {'name': 'x23', 'value': '0'},
            {'name': 'x24', 'value': '0'},
            {'name': 'x25', 'value': '0'},
            {'name': 'x26', 'value': '0'},
            {'name': 'x27', 'value': '0'},
            {'name': 'x28', 'value': '0'},
            {'name': 'x29', 'value': '0'},
            {'name': 'x30', 'value': '0'},
            {'name': 'x31', 'value': '0'},
        ]
    );
    // update the values of regs from data
    // useEffect(() => {
    //     if(data){
    //         setRegs(data.registers);
    //     }
    // }, [data]);

    return (
        <div className='registers'>
        <div className='table'>
            <div className='row'>
                <div className='regs-name-title'>Register</div>
                <div className='regs-value-value'>Value</div>
            </div>
            <div className='tbody'>
                {regs.map((reg, index) => (
                    <div key={index}>
                        <div className="row">
                            <div className='regs-name'>{reg.name}</div>
                            <div className='regs-value'>{reg.value}</div>
                        </div>
                    </div>
                ))}
            </div>
        </div>
        </div>
    );
}

export default Registers