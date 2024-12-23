import React, { useState, useContext } from "react";
import "./Mem.css";
import { DataContext } from "../context/DataContext.jsx";

const Mem = () => {
  const { mem } = useContext(DataContext);  // Getting memory from context
  const baseAddress = 0x10000;  // Starting address for memory
  const rowsPerPage = 20;  // Number of rows to display per page (8 bytes per row)
  const [startIndex, setStartIndex] = useState(0);  // Current page index

  // Convert memory object into an array grouped by lines of 8 bytes
  const memArray = Array.from({ length: Math.ceil(Object.keys(mem).length / 8) }, (_, i) => {
    const lineAddress = baseAddress + i * 8;  // Calculate the base address for this line
    const lineValues = Array.from({ length: 8 }).map((_, j) => {
      // Format the address as a hex string and retrieve the value from memory
      const memAddress = `0x${(lineAddress + j).toString(16)}`;  // Format address as hex
      const memValue = mem[memAddress];  // Get the value for this address
      return memValue ? parseInt(memValue, 16) : 0;  // Parse value to integer or default to 0
    });
    return { address: lineAddress, values: lineValues };  // Return row with address and values
  });

  // Handle "Next" page button
  const handleNextPage = () => {
    if (startIndex + rowsPerPage < memArray.length) {
      setStartIndex((prevIndex) => prevIndex + rowsPerPage);
    }
  };

  // Handle "Previous" page button
  const handlePreviousPage = () => {
    setStartIndex((prevIndex) => Math.max(0, prevIndex - rowsPerPage));
  };

  // Render memory rows
  const renderRows = () => {
    const rows = [];
    for (let i = startIndex; i < Math.min(memArray.length, startIndex + rowsPerPage); i++) {
      const { address, values } = memArray[i];
      rows.push(
        <div className="row" key={address}>
          <div className="mem-address">
            {address.toString(16).padStart(8, "0")}  {/* Format address in hex with padding */}
          </div>
          {values.map((value, j) => (
            <div key={j} className="mem-byte">
              0x{value.toString(16).padStart(2, "0")}  {/* Format byte values as hex with padding */}
            </div>
          ))}
        </div>
      );
    }
    return rows;
  };

  return (
    <div className="mem">
      <div className="row-title">
        <h1>Address</h1>
        <h1>Byte 0</h1>
        <h1>Byte 1</h1>
        <h1>Byte 2</h1>
        <h1>Byte 3</h1>
        <h1>Byte 4</h1>
        <h1>Byte 5</h1>
        <h1>Byte 6</h1>
        <h1>Byte 7</h1>
      </div>
      {renderRows()}  {/* Render memory rows */}
      <div className="buttons">
        <button onClick={handlePreviousPage} disabled={startIndex === 0}>
          Previous {rowsPerPage} Rows
        </button>
        <button
          onClick={handleNextPage}
          disabled={startIndex + rowsPerPage >= memArray.length}
        >
          Next {rowsPerPage} Rows
        </button>
      </div>
    </div>
  );
};

export default Mem;
