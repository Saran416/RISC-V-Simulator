import Editor from '../components/Editor'
import Navbar from '../components/Navbar'
import Registers from '../components/Registers'
import { useEffect, useState } from 'react'
import './About.css'

const Home = () => {
  const [isSmallScreen, setIsSmallScreen] = useState(window.innerWidth <= 1330);

  useEffect(() => {
    window.addEventListener('resize', () => {
      setIsSmallScreen(window.innerWidth <= 1330);
    });
  });

  return (
    <>
      <Navbar />
      <div className="main">
        <div className="about">
            <h3>📘 About</h3>
            <br/>
            <p>
                The RISC-V Simulator Web App is an interactive platform for writing, executing, and visualizing RISC-V assembly programs. 
                It simulates a complete CPU environment with a focus on cache behavior, register operations, and memory updates. 
                The simulator is written in C++, while the backend is built using Node.js and Express, and the frontend uses React.
            </p>
            <br/>

            <h3>⚙️ Features</h3>
            <br/>
            <p>
                • Supports basic <strong>RISC-V instruction types</strong>: R, I, S, etc.<br/>
                • Simulates <strong>32 general-purpose registers (x0–x31)</strong>, each <strong>64 bits wide</strong><br/>
                • Byte-addressable memory<br/>
                • Fully <strong>configurable cache</strong>:
                <ul>
                    <li>Cache size</li>
                    <li>Block size</li>
                    <li>Associativity</li>
                    <li>Replacement policy</li>
                    <li>Write policy</li>
                </ul>
                • Execution modes:
                <ul>
                    <li><strong>Run</strong> – execute the full program</li>
                    <li><strong>Step</strong> – execute one instruction at a time</li>
                    <li><strong>Reset</strong> – clear all states</li>
                </ul>
                • Real-time visualization of:
                <ul>
                    <li>Register values</li>
                    <li>Memory contents</li>
                    <li>Cache state</li>
                    <li>Logs and errors</li>
                </ul>
            </p>
            <br/>

            <h3>🧑‍💻 How to Use</h3>
            <br/>
            <p>
                1. <strong>Editor Page</strong> – Write or paste your RISC-V assembly code into the editor.<br/>
                2. Use the <strong>Run</strong>, <strong>Step</strong>, or <strong>Reset</strong> buttons to control execution.<br/>
                3. View current <strong>register values</strong> and <strong>logs</strong> in the right panel.<br/>
                4. Switch to the <strong>Memory</strong> page to inspect memory updates.<br/>
                5. Go to the <strong>Cache Configuration</strong> page to set up custom cache parameters.<br/>
                6. Make changes and re-run the program to see how cache performance is affected.
            </p>
            <br/>

            <h3>📂 GitHub Repository</h3>
            <br/>
            <p>
                🔗 <a href="https://github.com/Saran416/RISC-V-Simulator" target="_blank">https://github.com/Saran416/RISC-V-Simulator</a> <br /> 
            </p>
            <br/>

            <h3>🤝 Contributors</h3>
            <br/>
            <p>
                Built by <a href="https://github.com/lakshsidhu04" target="_blank">lakshsidhu04</a> and <a href="https://github.com/Saran416" target="_blank">Saran416</a><br/>
            </p>
        </div>
      </div>
    </>
  )
}

export default Home