import Editor from '../components/Editor'
import Navbar from '../components/Navbar'
import Registers from '../components/Registers'
import './Home.css'
import { useContext } from 'react'
import { DataContext } from '../context/DataContext.jsx'


const Home = () => {
  // const { updateData } = useContext(DataContext);

  // const getData = async () => {
  //   const response = await fetch('http://localhost:5000/data');
  //   const data = await response.json();
  //   console.log(data);
  //   updateData(data);
  // }

  return (
    <>
    <Navbar></Navbar>
    <div className="toolbar">
      <button className='run'>
        <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
          <path d="M8 5v14l11-7L8 5z"/>
        </svg>
      </button>
      <button className='step'>
        <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
          <path d="M8.29 16.29a1 1 0 001.42 0l4-4a1 1 0 000-1.42l-4-4a1 1 0 10-1.42 1.42L11.59 12l-3.3 3.29a1 1 0 000 1.42z"/>
        </svg>
      </button>
      <button className='stop'>
        <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
          <path d="M6 5h4v14H6V5zm8 0h4v14h-4V5z"/>
        </svg>
      </button>
    </div>
    <div className="main-area">
        <Editor />
        <Registers />
    </div>
    </>
  )
}

export default Home