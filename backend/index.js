const express = require('express');
const cors = require('cors');
const fs = require('fs');
const { spawn } = require('child_process');
const app = express();

app.use(cors());
app.use(express.json());

app.post('/getData', (req, res) => {
    const { code } = req.body;

    fs.writeFile('./Simulator/input.s', code, (err) => {
        if (err) {
            console.error('Error writing to file', err);
            res.status(500).json({ success: false, message: 'Internal Server Error' });
            return;
        }

        console.log('File written successfully');

        const simProcess = spawn('./riscv_sim', ['run'], { cwd: './Simulator' });
        let dataOutput = "";

        simProcess.stdout.on('data', (data) => {
            dataOutput += data;
        });

        simProcess.on('close', () => {
            const lines = dataOutput.split('\n');
            // register will be a map like "x0" : value
            let registers = {};
            let memory = {};
            
            let isRegisters = true;
            let registerCount = 0;
            let memCount = 0;

            for (let i = 0; i < lines.length; i++) {
                if (lines[i] === '') {
                    isRegisters = false;
                    continue;
                }
                if (isRegisters) {
                    registers[`x${registerCount}`] = lines[i];
                    registerCount++;
                } else {
                    memory[`0x${memCount}`] = lines[i];
                    memCount++;
                }
            }

            res.json({ success: true, registers: registers, memory: memory });
        });

        simProcess.on('error', (error) => {
            console.error('Error executing simulator', error);
            res.status(500).json({ success: false, message: 'Simulator execution failed' });
        });
    });
});

app.listen(5069, () => {
    console.log('Server running on port 5069');
});
