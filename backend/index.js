const express = require('express');
const cors = require('cors');
const fs = require('fs');
const { exec } = require('child_process');
const app = express();
const port = 3001;

app.use(cors());
app.use(express.json());

app.post('/login', (req, res) => {
    const { code } = req.body;
    if(read.success){
        fs.writeFile('code.txt', code, (err) => {
            if (err) {
                console.error('Error writing to file', err);
                res.status(500).json({ success: false, message: 'Internal Server Error' });
                return;
            }
        });
    }
    exec('g++ main.cpp -o main && ./main', (err, stdout, stderr) => {
        if (err) {
            console.error('Error executing file', err);
            res.status(500).json({ success: false, message: 'Execution Error' });
            return;
        }
        if (stderr) {
            console.error('Execution stderr', stderr);
            res.status(500).json({ success: false, message: 'Execution Error' });
            return;
        }
        res.json({ success: true, output: stdout });
    });
});