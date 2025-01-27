import React, { useState, useEffect, useContext } from 'react';
import CodeMirror from '@uiw/react-codemirror';
import { dracula } from '@uiw/codemirror-theme-dracula';
import { DataContext } from '../context/DataContext.jsx';
import runIcon from '../assets/run.svg';
import stepIcon from '../assets/step.svg';
import restartIcon from '../assets/restart.svg';
import uploadIcon from '../assets/upload.svg';
import { io } from "socket.io-client";
import { Decoration, ViewPlugin } from '@codemirror/view';
import './Editor.css';

const socket = io("http://localhost:3000");

const Editor = () => {
    const defaultText = `.data\n.text`;
    const [code, setCode] = useState(defaultText);
    const [hex, setHex] = useState('');
    const [highlightedLine, setHighlightedLine] = useState(1);
    const {updateRegs, updateMem, defaultInitialise, log, updateLog, err, updateErr, pc, updatePc, cacheConfig } = useContext(DataContext);
    socket.emit('hexInstructions', {code});

    useEffect(() => {
        if (code !== defaultText) {
            localStorage.setItem('curr_code', code);
        }
        const connectHandler = () => console.log("Connected to server");
        const responseHandler = (data) => {
            console.log(data)
            if(data.success){
                updateRegs(data.registers);
                updateMem(data.memory);
                updateLog(data.statuslog);
                updateErr(data.statuslog[0] !== 'E' && data.statuslog[0] !== 'C');
                setHighlightedLine(calculateHighlightedLine(data.gpc))
                updatePc(data.gpc);
            }
            else{
                updateLog(data.message);
                updateErr(true);
            }
        };

        const hexHandler = (data) => {
            // console.log("hex data" , data)
            if(data.success){
                let hexString = '';
                data.hexInstructions.forEach(instruction => {
                    hexString += instruction + '\n';
                });
                setHex(hexString);
            }
            else{
                updateErr(true);
                updateLog(data.message)
            }
        };
        
        socket.on("connect", connectHandler);
        socket.on("response", responseHandler);
        socket.on("hex_response", hexHandler)
    
        return () => {
            socket.off("connect", connectHandler);
            socket.off("response", responseHandler);
        };
    }, [code]);
    

    useEffect(() => {
        const savedCode = localStorage.getItem('curr_code');
        if (savedCode) {
            setCode(savedCode);
        }
    }, []);

    // Function to calculate the highlighted line based on the program counter (pc) and code content
    function calculateHighlightedLine(pc) {
        const lines = code.split('\n');
        const offset = lines.map(line => line.trim()[0] === '.' ? 1 : 0).reduce((acc, curr, index) => curr === 1 ? index + 1 : acc, 0) + 1;
        console.log("offset" , offset)
        console.log("pc" , pc);
        return Math.floor(pc / 4) + offset;
    }

    const lineHighlightPlugin = ViewPlugin.fromClass(
        class {
            constructor(view) {
                this.decorations = this.getDecorations(view);
            }
    
            getDecorations(view) {
                if (highlightedLine < 1 || highlightedLine > view.state.doc.lines) {
                    return Decoration.none;
                }
                const line = view.state.doc.line(highlightedLine);
                return Decoration.set([Decoration.line({ class: 'highlight-line' }).range(line.from)]);
            }
    
            update(update) {
                if (update.docChanged || update.viewportChanged) {
                    this.decorations = this.getDecorations(update.view);
                }
            }
        },
        {
            decorations: (v) => v.decorations,
        }
    );

    // run code 
    const runCode = () => {
        socket.emit("getData", { code, arg: "run", pc, cacheConfig });
        updatePc(0);
    };

    const stepCode = () => {
        socket.emit("getData", { code, arg: "step", pc, cacheConfig });
        updatePc((prevPc) => prevPc + 4);
    };

    const restart = () => {
        socket.emit("setzero");
        defaultInitialise();
        updatePc(0);
        updateErr(false);
        updateLog("Reset Successful");
        setHighlightedLine(calculateHighlightedLine(0));
    };

    const handleFileUpload = (event) => {
        const file = event.target.files[0];
        if (file) {
            if (file.name.endsWith('.s')) {
                const reader = new FileReader();
                reader.onload = (e) => {
                    setCode(e.target.result);
                    updateLog(`File ${file.name} uploaded successfully`);
                    updateErr(false);
                };
                reader.readAsText(file);
            } else {
                updateLog('Upload Error: Only .s files are supported');
                updateErr(true);
            }
        }
    };

    return (
        <div className="code-area">
            <div className="toolbar">
                <div className="file-upload">
                    <label htmlFor="file-upload" className="upload-label">
                        <img src={uploadIcon} alt="upload" className="upload-icon" />
                    </label>
                    <input
                        id="file-upload"
                        type="file"
                        accept=".s"
                        onChange={handleFileUpload}
                        style={{ display: 'none' }}
                    />
                </div>

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
                    extensions={[
                        dracula,
                        lineHighlightPlugin,
                    ]}
                    // extensions={[dracula, lineHighlightPlugin]}
                    onChange={(data) => setCode(data)}
                />
                <CodeMirror
                    value={hex}
                    extensions={[dracula]}
                    className="hexes"
                    editable={false}
                    basicSetup={{ lineNumbers: false }}
                />
            </div>
        </div>
    );
};

export default Editor;
