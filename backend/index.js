const express = require('express');
const cors = require('cors');
const fs = require('fs');
const { spawn } = require('child_process');
const app = express();

app.use(cors());
app.use(express.json());

app.post('/getData', (req, res) => {
    const { code , arg, pc} = req.body;
    console.log('Received code:', code);
    console.log('Received arg:', arg);
    console.log('Received pc:', pc);
    fs.writeFile('./Simulator/input.s', code, (err) => {
        if (err) {
            console.error('Error writing to file', err);
            res.status(500).json({ success: false, message: 'Internal Server Error' });
            return;
        }

        console.log('File written successfully');

        const simProcess = spawn('./riscv_sim', [arg,pc], { cwd: './Simulator' });
        console.log(`./riscv_sim ${arg} ${pc}`);
        let dataOutput = "";

        simProcess.stdout.on('data', (data) => {
            dataOutput += data;
        });
        
        simProcess.on('close', () => {
            const lines = dataOutput.split('\n');
            // Register and memory will be returned as key-value pairs
            let registers = {};
            let memory = {};

            let isRegisters = true;
            let registerCount = 0;
            let memCount = 0;

            for (let i = 0; i < lines.length; i++) {
                if (lines[i] === '') {
                    isRegisters = false;
                    console.log('Memory starts at line', i);
                    continue;
                }
                if (isRegisters) {
                    registers[`x${registerCount}`] = lines[i];
                    registerCount++;
                } else {
                    // Memory formatting as `0x10000`, `0x10001`, etc.
                    const address = `0x${(0x10000 + memCount).toString(16)}`;
                    memory[address] = lines[i];  // Map address to value
                    memCount++;
                }
            }

            // Send registers and memory to the client
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
