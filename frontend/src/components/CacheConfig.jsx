import React, { useState, useContext, useEffect } from 'react';
import './CacheConfig.css'; 
import { DataContext } from '../context/DataContext.jsx';

const CacheConfig = () => {
  const {cacheConfig, updateCacheConfig, hits, updateHits, misses, updateMisses} = useContext(DataContext);
  const {size, blockSize, associativity, writePolicy, replacementPolicy} = cacheConfig;
  const [accesses, setAccesses] = useState(0);
  const [hitRate, setHitRate] = useState('0%');

  useEffect(() => {
    setAccesses(Number(hits) + Number(misses));
    calculateHitRate();
  }, [hits, misses]);

  const calculateHitRate = () => {
    const rate =  Number(hits) + Number(misses) > 0 ? ((Number(hits) / (Number(hits) + Number(misses))) * 100).toFixed(2) + '%' : '0%';
    setHitRate(rate);
  };

  return (
    <div className="cache-config-container">
      <h2>Cache Configuration</h2>

      <div className="cache-settings">
        <div className="input-field">
          <label htmlFor="associativity">Cache Associativity (ways):</label>
          <input
            type="number"
            id="associativity"
            value={associativity}
            min="1"
            onChange={(e) => updateCacheConfig((prev) => ({ ...prev, associativity: Number(e.target.value) }))}
          />
        </div>

        <div className="input-field">
          <label htmlFor="blockSize">Block Size (bytes):</label>
          <input
            type="number"
            id="blockSize"
            value={blockSize}
            min="1"
            onChange={(e) => updateCacheConfig((prev) => ({ ...prev, blockSize: Number(e.target.value)}))}
          />
        </div>

        <div className="input-field">
          <label htmlFor="cacheSize">Cache Size (KB):</label>
          <input
            type="number"
            id="cacheSize"
            value={size}
            min="1"
            onChange={(e) => updateCacheConfig((prev) => ({ ...prev, s: Nizeumber(e.target.value)}))}
          />
        </div>

        <div className="dropdown">
          <label htmlFor="replacementPolicy">Replacement Policy:</label>
          <select
            id="replacementPolicy"
            value={replacementPolicy}
            onChange={(e) => updateCacheConfig((prev) => ({ ...prev, replacementPolicy: (e.target.value)}))}
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
            onChange={(e) => updateCacheConfig((prev) => ({ ...prev, writePolicy: (e.target.value)}))}
          >
            <option value="Write Back">Write Back</option>
            <option value="Write Through">Write Through</option>
          </select>
        </div>
      </div>

      <div className="cache-stats">
        <h2>Cache Statistics</h2>
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
