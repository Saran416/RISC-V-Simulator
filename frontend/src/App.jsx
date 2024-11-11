import React from 'react'
import './App.css'
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom'
import Home from './pages/Home.jsx'
import Memory from './pages/Memory.jsx'
import { Navigate } from 'react-router-dom'

const App = () => {
  return (  
    <Router>
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/memory" element={<Memory />} />
        <Route path='*' element={<Navigate to='/' />} />
      </Routes>
    </Router>
  )
}

export default App