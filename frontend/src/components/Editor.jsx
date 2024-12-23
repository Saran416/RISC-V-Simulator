import React, { useState, useEffect, useContext } from 'react';
import CodeMirror from '@uiw/react-codemirror';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { DataContext } from '../context/DataContext.jsx';
import runIcon from '../assets/run.svg';
import stepIcon from '../assets/step.svg';
import restartIcon from '../assets/restart.svg';
import { EditorView, Decoration, ViewPlugin } from '@codemirror/view';
import './Editor.css';

const Editor = () => {
    const defaultText = `.data\n.text`;
    const [code, setCode] = useState(defaultText);
    let offset = 0;
    let number = 2;
    const { updateRegs, updateMem, defaultInitialise, log, updateLog, err, updateErr, pc, updatePc } = useContext(DataContext);
    const [highlightedLine, setHighlightedLine] = useState(1);


    useEffect(() => {
        if (code !== defaultText) {
            localStorage.setItem('curr_code', code);
        }
    }, [code]);

    useEffect(() => {
        const savedCode = localStorage.getItem('curr_code');
        if (savedCode) {
            setCode(savedCode);
        }
    }, []);


    const findTextLine = () => {
        const lines = code.split('\n'); // Split the code into lines
        number = lines.length;
        console.log("Number of line" + number);
        for (let i = 0; i < lines.length; i++) {
            if (lines[i].includes('.text')) {
                console.log("found" + (i + 1))
                return i + 1; // Return the 1-indexed line number
            }
        }
        return 1; // Return null if ".text" is not found
    };

    useEffect(() => {
        offset = findTextLine();
        console.log(offset);
        // Update the highlighted line whenever `pc` changes
        const newLineNumber = (pc / 4) + 1 + offset; // Assuming 4 bytes per line
        setHighlightedLine(newLineNumber);
    }, [pc, code]);

    // Line highlighting plugin
    const lineHighlightPlugin = ViewPlugin.fromClass(
        class {
            constructor(view) {
                this.decorations = this.createHighlight(view, highlightedLine);
            }

            update(update) {
                if (update.docChanged || update.transactions.length > 0) {
                    this.decorations = this.createHighlight(update.view, highlightedLine);
                }
            }

            createHighlight(view, lineNumber) {
                const line = view.state.doc.line(lineNumber);
                console.log('Highlighting line:', line);
                return Decoration.set([
                    Decoration.line({ attributes: { class: 'highlight-line' } }).range(line.from),
                ]);
            }
        },
        {
            decorations: (plugin) => plugin.decorations,
        }
    );

    const runCode = async () => {
        try {
            const response = await fetch('http://localhost:3000/getData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code: code, arg: 'run', pc: pc }),
            });

            if (!response.ok) {
                throw new Error('Failed to fetch data');
            }

            const { registers, memory, statuslog } = await response.json();
            // console.log(statuslog);

            if (statuslog[0] === 'E' || statuslog[0] === 'C') {
                updateLog(statuslog);
                updateErr(false);
                updatePc(0);
            } else {
                updateLog(statuslog);
                updateErr(true);
            }

            if (Object.keys(registers).length !== 0 && (statuslog[0] === 'E' || statuslog[0] === 'C')) {
                updateRegs(registers);
                updateMem(memory);
            }
        } catch (error) {
            updateErr(true);
            updateLog(error.message);
            console.error('Error running code:', error);
        }
    };

    const stepCode = async () => {
        try {
            const response = await fetch('http://localhost:3000/getData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code: code, arg: 'step', pc: pc }),
            });

            if (!response.ok) {
                throw new Error('Failed to fetch data');
            }

            const { registers, memory, statuslog } = await response.json();
            // console.log(statuslog);

            if (statuslog[0] === 'E' || statuslog[0] === 'C') {
                updateLog(statuslog);
                updateErr(false);
                updatePc(pc + 4);
            } else if (statuslog === '') {
                updateLog('Nothing to step');
                updateErr(true);
            } else {
                updateLog(statuslog);
                updateErr(true);
            }

            if (Object.keys(registers).length !== 0 && (statuslog[0] === 'E' || statuslog[0] === 'C')) {
                updateRegs(registers);
                updateMem(memory);
            }
        } catch (error) {
            updateErr(true);
            updateLog(error.message);
            console.error('Error running code:', error);
        }
    };

    const restart = async () => {
        updateLog('Reset Successful');
        updateErr(false);
        await fetch('http://localhost:3000/setzero', { method: 'GET' });
        defaultInitialise();
        updatePc(0);
    };

    return (
        <div className="code-area">
            <div className="toolbar">
                <div className="toolbar-buttons">
                    <button className="run" onClick={runCode}>
                        <img src={runIcon} alt="Run" className="icon" />
                    </button>
                    <button className="step" onClick={stepCode}>
                        <img src={stepIcon} alt="Step" className="icon" />
                    </button>
                    <button className="stop" onClick={restart}>
                        <img src={restartIcon} alt="Restart" className="icon" />
                    </button>
                </div>
                <div className="toolbar-log">
                    {err ? <div className="error">{log}</div> : <div className="log">{log}</div>}
                </div>
            </div>
            <div className="editor">
                <CodeMirror
                    value={code}
                    extensions={[dracula, lineHighlightPlugin]}
                    onChange={(data) => setCode(data)}
                />
            </div>
        </div>
    );
};

export default Editor;
