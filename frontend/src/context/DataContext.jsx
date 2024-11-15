import { createContext, useCallback, useEffect, useState } from "react";

export const DataContext = createContext()

export const DataContextProvider = ({ children }) => {
    const [regs, setRegs] = useState({
        "x0": 0,
        "x1": 0,
        "x2": 0,
        "x3": 0,
        "x4": 0,
        "x5": 0,
        "x6": 0,
        "x7": 0,
        "x8": 0,
        "x9": 0,
        "x10": 0,
        "x11": 0,
        "x12": 0,
        "x13": 0,
        "x14": 0,
        "x15": 0,
        "x16": 0,
        "x17": 0,
        "x18": 0,
        "x19": 0,
        "x20": 0,
        "x21": 0,
        "x22": 0,
        "x23": 0,
        "x24": 0,
        "x25": 0,
        "x26": 0,
        "x27": 0,
        "x28": 0,
        "x29": 0,
        "x30": 0,
        "x31": 0,
    });
    const [mem, setMem] = useState({});
    
    

    const updateRegs = useCallback((data) => {
        // is data is not an empty object add
        if(Object.keys(data).length !== 0){
            setRegs(data); 
        }
         // Update registers data
    }, []);

    const updateMem = useCallback((data) => {
        setMem(data);  // Update memory data
    }, []);

    return (
        <DataContext.Provider value={{ regs, mem, updateMem, updateRegs }}>
            {children}
        </DataContext.Provider>
    );
};
