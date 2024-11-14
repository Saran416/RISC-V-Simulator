import CodeMirror from '@uiw/react-codemirror';
import React, { useState, useEffect } from 'react';
import './Editor.css';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { useContext } from 'react';
import { DataContext } from '../context/DataContext.jsx';
// import { useContext } from 'react'
// import { DataContext } from '../context/DataContext.jsx'

const Editor = () => {
    // Default code text without leading indentation or newlines
    const defaultText = `.data
.text`;

    // Set initial code in the state
    const [code, setCode] = useState(defaultText);
    
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
        } catch (error) {
            console.error('Error running code:', error);  // Handle any errors
        }
    };

    return (
        <div className='code-area'>
            <div className="toolbar">
                <button className='run' onClick={runCode}>
                    <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
                    <path d="M8 5v14l11-7L8 5z"/>
                    </svg>
                </button>
                <button className='step'>
                    <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
                    <path d="M8.29 16.29a1 1 0 001.42 0l4-4a1 1 0 000-1.42l-4-4a1 1 0 10-1.42 1.42L11.59 12l-3.3 3.29a1 1 0 000 1.42z"/>
                    </svg>
                </button>
                <button className='stop'>
                    <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
                    <path d="M6 5h4v14H6V5zm8 0h4v14h-4V5z"/>
                    </svg>
                </button>
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
