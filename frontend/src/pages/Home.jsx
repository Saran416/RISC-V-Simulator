import Editor from '../components/Editor'
import Navbar from '../components/Navbar'
import Registers from '../components/Registers'
import { useEffect, useState } from 'react'
import './Home.css'



const Home = () => {
  // const { updateData } = useContext(DataContext);

  // const getData = async () => {
  //   const response = await fetch('http://localhost:5000/data');
  //   const data = await response.json();
  //   console.log(data);
  //   updateData(data);
  // }

  // const runFile = async () => {
  const [isSmallScreen, setIsSmallScreen] = useState(window.innerWidth <= 1330);

  useEffect(() => {
    window.addEventListener('resize', () => {
      setIsSmallScreen(window.innerWidth <= 1330);
    });
  });

  return (
    <>
      <Navbar />
      <div className="main-area">
        {isSmallScreen ? (
          <>
            <Registers className='registers' />
            <Editor className='editor-home' />
          </>
        ) : (
          <>
            <Editor className='editor-home' />
            <Registers className='registers' />
          </>
        )}
      </div>
    </>
  )
}

export default Home