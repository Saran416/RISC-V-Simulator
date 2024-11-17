import CodeMirror from '@uiw/react-codemirror';
import React, { useState, useEffect, useContext, useRef } from 'react';
import './Editor.css';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { DataContext } from '../context/DataContext.jsx';
import runIcon from '../assets/run.svg';
import stepIcon from '../assets/step.svg';
import restartIcon from '../assets/restart.svg';

const Editor = () => {
    // Default code text without leading indentation or newlines
    const defaultText = `.data
.text`;
    const [code, setCode] = useState(defaultText);  // Set initial code in the state
    const [pc, setPc] = useState(0);  // Initialize program counter state
    const { updateRegs, updateMem, defaultInitialise , log, updateLog, err, updateErr} = useContext(DataContext);

    const prevPcRef = useRef(pc);  // To store the previous pc value for debugging purposes

    // Save code to local storage whenever it changes
    useEffect(() => {
        if (code !== defaultText) {
            localStorage.setItem('curr_code', code);
        }
    }, [code]);


    // Load code and program counter from local storage if available
    useEffect(() => {
        const savedCode = localStorage.getItem('curr_code');
        if (savedCode) {
            setCode(savedCode);
        }
    }, []); 

    const runCode = async () => {
        try {
            const response = await fetch('http://localhost:5069/getData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code: code, arg: 'run', pc: pc }),
            });
            
            if (!response.ok) {
                throw new Error('Failed to fetch data');
            }

            const { registers, memory, statuslog } = await response.json(); // Extract registers and memory from response
            // console.log("Printing registers and memory:");
            // console.log(registers);  // Print registers
            console.log(statuslog);  // Print memory

            if(statuslog[0] === 'E'){
                updateLog(statuslog);
                updateErr(false);
            }
            else
            {
                updateLog(statuslog)
                updateErr(true);
            }

            if (Object.keys(registers).length !== 0) {
                updateRegs(registers);   // Update context with registers
                updateMem(memory);
                updateErr(false);  // Reset error state
                updateLog(statuslog);
            }
        } catch (error) {
            updateErr(true);  // Set error state
            updateLog(error.message);
            console.error('Error running code:', error);  // Handle any errors
        }
    };

    const stepCode = async () => {
        try {
            // console.log(`Current PC before step: ${pc}`);
            const response = await fetch('http://localhost:5069/getData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code: code, arg: 'step', pc: pc }),
            });

            if (!response.ok) {
                throw new Error('Failed to fetch data');
            }

            const { registers, memory, statuslog } = await response.json(); // Extract registers and memory from response
            // console.log("Printing registers and memory:");
            // console.log(registers);  // Print registers
            console.log(statuslog);  // Print memory

            if(statuslog[0] === 'E'){
                updateLog(statuslog);
                updateErr(false);
            }
            // if statuslog is empty string
            else if(statuslog === ""){
                updateLog("Nothing to step");
                updateErr(true);
            }
            else
            {
                updateLog(statuslog)
                updateErr(true);
            }


            if (Object.keys(registers).length !== 0)  {
                updateRegs(registers);   // Update context with registers
                updateMem(memory);
                updateErr(false);  // Reset error state
                updateLog(statuslog);
            }
        } catch (error) {
            updateErr(true);  // Set error state
            updateLog(error.message);
            console.error('Error running code:', error);  // Handle any errors
        }
    };

    const restart =     async () => {
        updateLog('Reset Successful');
        updateErr(false);
        const response = await fetch('http://localhost:5069/setzero', {
            method: 'GET',
        });
        defaultInitialise();
    };


    return (
        <div className='code-area'>
            <div className="toolbar">
                <div className="toolbar-buttons">
                    <button className='run' onClick={runCode}>
                        <img src={runIcon} alt="Run" className='icon' />
                    </button>
                    <button className='step' onClick={stepCode}>
                        <img src={stepIcon} alt="Step" className='icon' />
                    </button>
                    <button className='stop' onClick={restart}>
                        <img src={restartIcon} alt="Restart" className='icon' />
                    </button>
                </div>
                <div className="toolbar-log">
                    {err ? <div className="error">{log}</div> : <div className="log">{log}</div>}
                </div>
            </div>
            <div className="editor">
                <CodeMirror
                    value={code}
                    options={{
                        mode: 'python',  // Set the language mode
                        lineNumbers: true,   // Show line numbers
                        lineWrapping: true,  // Enable line wrapping (optional)
                        matchBrackets: true,  // Highlight matching brackets
                        autoCloseBrackets: true,  // Auto close brackets
                    }}
                    onChange={(data) => {
                        setCode(data); // Update the state with the code string
                    }}
                    theme={dracula} // Set the theme
                />
            </div>
        </div>
    );
};

export default Editor;
