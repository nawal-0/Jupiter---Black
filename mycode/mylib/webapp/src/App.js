import React from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import Home from './pages/Home';
import HeaderFooter from './components/header';
import './data/styles.css';

function App() {
  return (
    <Router>
      <div className="App">
        <HeaderFooter />
      </div>

      <Routes>
        <Route path="/" element={<Home />} />
      </Routes>
    </Router>
  );
}

export default App;
