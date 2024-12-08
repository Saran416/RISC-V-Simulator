import React from 'react';
import { Link } from 'react-router-dom';
import './Navbar.css'; // Assuming you have some CSS for styling

const Navbar = () => {
    return (
        <nav className="navbar">
            <Link to="/" className='nav-title'>RISC-V</Link>
            <Link to="/editor" className="nav-link">Editor</Link>
            <Link to="/memory" className="nav-link">Memory</Link>
        </nav>
    );
};

export default Navbar;