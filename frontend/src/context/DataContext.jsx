import React, { useState, useCallback } from 'react';
import { createContext } from 'react';

export const DataContext = createContext({
    regs: {},
    mem: {},
    updateRegs: () => {},
    updateMem: () => {},
    defaultInitialise: () => {},
    log: '',
    updateLog: () => {},
    err: false,
    updateErr: () => {},
});

export const DataContextProvider = ({ children }) => {
    // Initialize registers state
    const [regs, setRegs] = useState({
        "x0": "0x0",
        "x1": "0x0",
        "x2": "0x0",
        "x3": "0x0",
        "x4": "0x0",
        "x5": "0x0",
        "x6": "0x0",
        "x7": "0x0",
        "x8": "0x0",
        "x9": "0x0",
        "x10": "0x0",
        "x11": "0x0",
        "x12": "0x0",
        "x13": "0x0",
        "x14": "0x0",
        "x15": "0x0",
        "x16": "0x0",
        "x17": "0x0",
        "x18": "0x0",
        "x19": "0x0",
        "x20": "0x0",
        "x21": "0x0",
        "x22": "0x0",
        "x23": "0x0",
        "x24": "0x0",
        "x25": "0x0",
        "x26": "0x0",
        "x27": "0x0",
        "x28": "0x0",
        "x29": "0x0",
        "x30": "0x0",
        "x31": "0x0",
    });

    // Initialize memory starting from 0x10000 and set values to 0
    const initialMemory = Array.from({ length: 0x400 }).reduce((acc, _, idx) => {
        const address = (0x10000 + idx).toString(16);  // Ensure hex formatting
        acc[`0x${address}`] = 0;  // Use the address in the correct hex format as the key
        return acc;
    }, {});
    
    const [mem, setMem] = useState(initialMemory);

    const [log,setLog] = useState('');  // Initialize log state

    const [err, setErr] = useState(false);  // Initialize error state

    const [pc, setPc] = useState(0);  // Initialize program counter

    const updatePc = useCallback((data) => {
        setPc(data);
    }, []);

    // Update registers state
    const updateRegs = useCallback((data) => {
        if (Object.keys(data).length !== 0) {
            setRegs(data);
        }
    }, []);

    // Update memory state
    const updateMem = useCallback((data) => {        
        setMem(data);
    }, []);

    const updateLog = useCallback((data) => {
        setLog(data);
    }, []);

    const updateErr = useCallback((data) => {
        setErr(data);
    }, []);

    const [cacheConfig, setCacheConfig] = useState({
        size: 1024,
        blockSize: 64,
        associativity: 4,
        replacementPolicy: "LRU",
        writePolicy: "Write Back"
    });

    const updateCacheConfig = useCallback((data)=>{
        setCacheConfig(data);
    } , []);

    const [hits, setHits] = useState(0);
    const [misses, setMisses] = useState(0);

    const updateHits = useCallback((data)=>{
        setHits(data)
    }, []);

    const updateMisses = useCallback((data)=>{
        setMisses(data)
    },[]);

    const defaultInitialise = () =>{
        setRegs({
            "x0": "0x0",
            "x1": "0x0",
            "x2": "0x0",
            "x3": "0x0",
            "x4": "0x0",
            "x5": "0x0",
            "x6": "0x0",
            "x7": "0x0",
            "x8": "0x0",
            "x9": "0x0",
            "x10": "0x0",
            "x11": "0x0",
            "x12": "0x0",
            "x13": "0x0",
            "x14": "0x0",
            "x15": "0x0",
            "x16": "0x0",
            "x17": "0x0",
            "x18": "0x0",
            "x19": "0x0",
            "x20": "0x0",
            "x21": "0x0",
            "x22": "0x0",
            "x23": "0x0",
            "x24": "0x0",
            "x25": "0x0",
            "x26": "0x0",
            "x27": "0x0",
            "x28": "0x0",
            "x29": "0x0",
            "x30": "0x0",
            "x31": "0x0",
        });
        setMem(initialMemory);
    }

    return (
        <DataContext.Provider value={{ err, regs, mem, log, pc, updateLog,updateMem, updateRegs, defaultInitialise, updateErr, updatePc, cacheConfig, updateCacheConfig, hits, updateHits, misses, updateMisses 
         }}>
            {children}
        </DataContext.Provider>
    );
};
