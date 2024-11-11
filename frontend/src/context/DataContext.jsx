import { createContext, useCallback, useEffect, useState } from "react";

export const DataContext = createContext()

export const DataContextProvider = ({children})=>{
    const [data,setData] = useState(null);

    // useEffect(()=>{
    //     if(temp){
    //         // console.log('User Data', JSON.parse(temp))
    //         setUser(JSON.parse(temp))
    //     }
    // },[])

    const updateData = useCallback((info)=>{
        setData(info)
    },[])

    return (
        <DataContext.Provider value={{data, updateData}}>
            {children}
        </DataContext.Provider>
    )
} 