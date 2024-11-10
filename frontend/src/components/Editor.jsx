import CodeMirror from '@uiw/react-codemirror';
import React, { useState, useEffect } from 'react';
import './Editor.css';
import { dracula } from '@uiw/codemirror-theme-dracula';

const Editor = () => {
    // Default code text without leading indentation or newlines
    const defaultText = `.data
.text`;

    // Set initial code in the state
    const [code, setCode] = useState(defaultText);

    // Effect to set initial value (optional if you want to modify defaultText dynamically)
    useEffect(() => {
        setCode(defaultText);
    }, []);

    return (
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
    );
};

export default Editor;
