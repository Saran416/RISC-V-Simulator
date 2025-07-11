import React from 'react';
import { Link } from 'react-router-dom';
import './Navbar.css';

const Navbar = () => {
    return (
        <nav className="navbar">
            <div className='nav-title'>
                <Link to="/">RISC-V</Link>
            </div>
            <div className="nav-link">
                <Link to="/editor">Editor</Link>
            </div>
            <div className="nav-link">
                <Link to="/memory">Memory</Link>
            </div>
            <div className="nav-link">
                <Link to="/cache">Cache</Link>
            </div>
        </nav>
    );
};

export default Navbar;