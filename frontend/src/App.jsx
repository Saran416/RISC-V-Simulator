import React from 'react'
import './App.css'
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom'
import Home from './pages/Home.jsx'
import Memory from './pages/Memory.jsx'
import Cache from './pages/Cache.jsx'
import { Navigate } from 'react-router-dom'
import { DataContextProvider } from './context/DataContext.jsx'

const App = () => {
  return (  
    <DataContextProvider>
      <Router>
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/memory" element={<Memory />} />
          <Route path="/cache" element={<Cache/>}/>
          <Route path='*' element={<Navigate to='/' />} />
        </Routes>
      </Router>
    </DataContextProvider>
  )
}

export default App