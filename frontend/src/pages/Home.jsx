import Editor from '../components/Editor'
import Navbar from '../components/Navbar'
import Registers from '../components/Registers'
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

  return (
    <>
    <Navbar></Navbar>
    <div className="main-area">
        <Editor className='editor-home'/>
        <Registers  className='registers'/>
    </div>
    </>
  )
}

export default Home