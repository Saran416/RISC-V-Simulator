import React from 'react'
import Editor from '../components/Editor'
import Navbar from '../components/Navbar'
import Registers from '../components/Registers'
import './Home.css'

const Home = () => {
  return (
    <>
    <Navbar></Navbar>
    <div className="main-area">
        <Editor />
        <Registers />
    </div>
    </>
  )
}

export default Home