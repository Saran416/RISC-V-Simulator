import React, { useState } from 'react';
import './CacheConfig.css'; 

const CacheConfig = () => {
  const [isCacheEnabled, setIsCacheEnabled] = useState(false);
  const [cacheAssociativity, setCacheAssociativity] = useState(1); 
  const [blockSize, setBlockSize] = useState(16); 
  const [cacheSize, setCacheSize] = useState(64);
  const [replacementPolicy, setReplacementPolicy] = useState('LRU');
  const [writePolicy, setWritePolicy] = useState('Write Back');

  const [hits, setHits] = useState(0);
  const [accesses, setAccesses] = useState(0);
  const [misses, setMisses] = useState(0);
  const [hitRate, setHitRate] = useState('0%');

  const calculateHitRate = () => {
    const rate = accesses > 0 ? ((hits / accesses) * 100).toFixed(2) + '%' : '0%';
    setHitRate(rate);
  };

  const toggleCache = () => {
    setIsCacheEnabled(!isCacheEnabled);
    setHits(0);
    setAccesses(0);
    setMisses(0);
    setHitRate('0%');
  };

  return (
    <div className="cache-config-container">
      <h2>Cache Configuration</h2>

      <div className="cache-toggle">
        <label>Cache Enabled:</label>
        <input type="checkbox" checked={isCacheEnabled} onChange={toggleCache} />
      </div>

      {isCacheEnabled && (
        <div className="cache-settings">
          <div className="input-field">
            <label htmlFor="associativity">Cache Associativity (ways):</label>
            <input
              type="number"
              id="associativity"
              value={cacheAssociativity}
              min="1"
              onChange={(e) => setCacheAssociativity(Number(e.target.value))}
            />
          </div>

          <div className="input-field">
            <label htmlFor="blockSize">Block Size (bytes):</label>
            <input
              type="number"
              id="blockSize"
              value={blockSize}
              min="1"
              onChange={(e) => setBlockSize(Number(e.target.value))}
            />
          </div>

          <div className="input-field">
            <label htmlFor="cacheSize">Cache Size (KB):</label>
            <input
              type="number"
              id="cacheSize"
              value={cacheSize}
              min="1"
              onChange={(e) => setCacheSize(Number(e.target.value))}
            />
          </div>

          <div className="dropdown">
            <label htmlFor="replacementPolicy">Replacement Policy:</label>
            <select
              id="replacementPolicy"
              value={replacementPolicy}
              onChange={(e) => setReplacementPolicy(e.target.value)}
            >
              <option value="LRU">LRU (Least Recently Used)</option>
              <option value="FIFO">FIFO (First In First Out)</option>
              <option value="Random">Random</option>
            </select>
          </div>

          <div className="dropdown">
            <label htmlFor="writePolicy">Write Policy:</label>
            <select
              id="writePolicy"
              value={writePolicy}
              onChange={(e) => setWritePolicy(e.target.value)}
            >
              <option value="Write Back">Write Back</option>
              <option value="Write Through">Write Through</option>
            </select>
          </div>
        </div>
      )}

      <div className="cache-stats">
        <h3>Cache Statistics</h3>
        <div className="stat-item">
          <label>Accesses:</label>
          <span>{accesses}</span>
        </div>
        <div className="stat-item">
          <label>Hits:</label>
          <span>{hits}</span>
        </div>
        <div className="stat-item">
          <label>Misses:</label>
          <span>{misses}</span>
        </div>
        <div className="stat-item">
          <label>Hit Rate:</label>
          <span>{hitRate}</span>
        </div>
      </div>
    </div>
  );
};

export default CacheConfig;
