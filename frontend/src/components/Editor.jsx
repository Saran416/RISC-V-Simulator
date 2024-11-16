import CodeMirror from '@uiw/react-codemirror';
import React, { useState, useEffect } from 'react';
import './Editor.css';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { useContext } from 'react';
import { DataContext } from '../context/DataContext.jsx';
import runIcon from '../assets/run.svg';
import stepIcon from '../assets/step.svg';
import restartIcon from '../assets/restart.svg';

const Editor = () => {
    // Default code text without leading indentation or newlines
    const defaultText = `.data
.text`;

    const [log, setLog] = useState('');  // Initialize log state
    const [code, setCode] = useState(defaultText);  // Set initial code in the state
    const [err, setErr] = useState(false);  // Initialize error state
    const [pc, setPc] = useState(0);  // Initialize program counter state
    const { updateRegs, updateMem, defaultInitialise } = useContext(DataContext);

    // Save code to local storage whenever it changes
    useEffect(() => {
        if (code !== defaultText) {
            localStorage.setItem('code', code);
        }
        if (pc !== 0) {
            localStorage.setItem('currpc', pc);
        }
    }, [code, pc]);

    // Load code and program counter from local storage if available
    useEffect(() => {
        const savedCode = localStorage.getItem('code');
        if (savedCode) {
            setCode(savedCode);
        }

        const savedPc = localStorage.getItem('currpc');
        if (savedPc) {
            setPc(parseInt(savedPc, 10));
            console.log(`pc ${pc}`)
        }

        console.log(`Initial PC on load: ${pc}`);  // This will log the correct value after it's updated
    }, []); // Empty dependency array to ensure this only runs once

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

            const { registers, memory } = await response.json(); // Extract registers and memory from response
            console.log("Printing registers and memory:");
            console.log(registers);  // Print registers

            if (registers['x0'][0] === 'L') {
                setErr(true);
                setLog(registers["x0"]);
            } else {
                updateRegs(registers);   // Update context with registers
                updateMem(memory);
                setErr(false);  // Reset error state
                setLog("Code executed successfully!");
            }
        } catch (error) {
            setErr(true);  // Set error state
            setLog(error.message);
            console.error('Error running code:', error);  // Handle any errors
        }
    };

    const stepCode = async () => {
        try {
            console.log(`Current PC before step: ${pc}`);
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

            const { registers, memory } = await response.json(); // Extract registers and memory from response
            console.log("Printing registers and memory:");
            console.log(registers);  // Print registers

            if (registers['x0'][0] === 'L') {
                setErr(true);
                setLog(registers["x0"]);
            } else {
                updateRegs(registers);   // Update context with registers
                updateMem(memory);
                setErr(false);  // Reset error state
                setLog("Code executed successfully!");
            }

            // Ensure the pc is correctly updated
            console.log(`Current PC after step: ${pc}`);
            setPc(prevPc => prevPc + 4); // Using functional update to prevent potential stale state issues
        } catch (error) {
            setErr(true);  // Set error state
            setLog(error.message);
            console.error('Error running code:', error);  // Handle any errors
        }
    };

    const restart = () => {
        setLog('');
        setPc(0); 
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
