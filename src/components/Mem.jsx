import React, { useEffect } from 'react';
import './Mem.css';

const Mem = () => {
  const [mem, setMem] = React.useState(new Array(0x50000).fill(0)); // Initialize memory with dummy values
  const baseAddress = 0x10000;
  const rowsPerPage = 20;
  const [startIndex, setStartIndex] = React.useState(0);

  useEffect(() => {
    // Simulate memory update or initialization if needed.
  }, []);

  const handleNextPage = () => {
    setStartIndex(prevIndex => prevIndex + rowsPerPage);
  };

  const handlePreviousPage = () => {
    setStartIndex(prevIndex => Math.max(0, prevIndex - rowsPerPage));
  };

  const renderRows = () => {
    const rows = [];
    for (let i = startIndex; i < Math.min(mem.length / 8, startIndex + rowsPerPage); i++) {
      const currentAddress = baseAddress + i * 8;
      const row = (
        <div className="row" key={currentAddress}>
          <div className='mem-address'>{currentAddress.toString(16).padStart(5, '0')}</div>
          {Array.from({ length: 8 }).map((_, j) => (
            <div key={j} className='mem-byte'>
              {mem[currentAddress + j] ?? 0}
            </div>
          ))}
        </div>
      );
      rows.push(row);
    }
    return rows;
  };

  return (
    <div className='mem'>
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
      {renderRows()}
      <div className="buttons">
        <button onClick={handlePreviousPage} disabled={startIndex === 0}>Previous 100 Rows</button>
        <button onClick={handleNextPage} disabled={startIndex + rowsPerPage * 8 >= mem.length}>Next 100 Rows</button>
      </div>
    </div>
  );
};

export default Mem;
