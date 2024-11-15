import CodeMirror from '@uiw/react-codemirror';
import React, { useState, useEffect } from 'react';
import './Editor.css';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { useContext } from 'react';
import { DataContext } from '../context/DataContext.jsx';
import runIcon from '../assets/run.svg';
import stepIcon from '../assets/step.svg';
import stopIcon from '../assets/stop.svg';

const Editor = () => {
    // Default code text without leading indentation or newlines
    const defaultText = `.data
.text`;
    const [log, setLog] = useState('');  // Initialize log state

    // Set initial code in the state
    const [code, setCode] = useState(defaultText);

    const [err, setErr] = useState(false);  // Initialize error state
    
    const { updateRegs, updateMem } = useContext(DataContext);
    // Effect to set initial value (optional if you want to modify defaultText dynamically)
    // useEffect(() => {
    //     setCode(defaultText);
    // }, []);
    
    const runCode = async () => {
        try {
            const response = await fetch('http://localhost:5069/getData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code: code }),
            });

            if (!response.ok) {
                throw new Error('Failed to fetch data');
            }

            const { registers, memory } = await response.json(); // Extract registers and memory from response
            console.log("Printing registers and memory:");
            console.log(registers);  // Print registers
            updateRegs(registers);   // Update context with registers
            updateMem(memory);       // Update context with memory
            console.log('memory updated\n');
        } catch (error) {
            setErr(true);  // Set error state
            setLog(error.message);
            console.error('Error running code:', error);  // Handle any errors
        }
        setErr(false);  // Reset error state
        setLog("Code executed successfully!");  // Set log message
    };

    return (
        <div className='code-area'>
            <div className="toolbar">
            <div className="toolbar-buttons">
                <button className='run' onClick={runCode}>
                    <img src={runIcon} alt="Run" className='icon'/>
                </button>
                <button className='step'>
                    <img src={stepIcon} alt="Step" className='icon'/>
                </button>
                <button className='stop'>
                    <img src={stopIcon} alt="Stop" className='icon'/>
                </button>
            </div>
            <div className="toolbar-log">
                {err? <div className="error">Error: Invalid code</div> : <div className="log">{log}</div>}
            </div>
            </div>
            <div className="editor">
                <CodeMirror
                    value={code}
                    options={{
                        mode: 'python',  // Set the language mode    // Set the theme
                        lineNumbers: true,   // Show line numbers
                        lineWrapping: true,  // Enable line wrapping (optional)
                        matchBrackets: true,  // Highlight matching brackets
                        autoCloseBrackets: true,  // Auto close brackets
                    }}
                    onChange={(data) => {
                        console.log(data); // Log the updated code string
                        setCode(data); // Update the state with the code string
                    }}
                    theme={dracula} // Set the theme
                />
            </div>
        </div>

    );
};

export default Editor;
