import React from 'react'
import Navbar from '../components/Navbar'
import Mem from '../components/Mem'

const Memory = () => {
  return (
    <>
    <Navbar></Navbar>
    <div className="main-mem-area">
      <Mem></Mem>
    </div>
    </>
  )
}

export default Memory