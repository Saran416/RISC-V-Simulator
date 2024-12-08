const express = require('express');
const cors = require('cors');
const fs = require('fs').promises;  // Use the promise-based version of fs
const { spawn } = require('child_process');
const app = express();

app.use(cors());
app.use(express.json());

app.post('/getData', async (req, res) => {
    const { code, arg, pc } = req.body;
    let gpc = pc;
    console.log('Received code:', code);
    console.log('Received arg:', arg);
    console.log('Received pc:', gpc);

    try {
        // Asynchronously write the code to the file
        await fs.writeFile('./Simulator/input.s', code);
        console.log('File written successfully');

        const simProcess = spawn('./riscv_sim', [arg, gpc], { cwd: './Simulator' });
        console.log(`./riscv_sim ${arg} ${gpc}`);

        let dataOutput = "";

        if (arg === 'step') {
            console.log("checking for step");
            gpc += 4;
        }

        if (arg === 'run') {
            console.log("checking for run");
            gpc = 0;
        }

        simProcess.stdout.on('data', (data) => {
            dataOutput += data;
        });

        // Wait for the simulation process to close and handle the output
        simProcess.on('close', () => {
            const lines = dataOutput.split('\n');
            let registers = {};
            let memory = {};
            let statuslog = "";
            let isRegisters = true;
            let registerCount = 0;
            let memCount = 0;

            statuslog = lines[0];
            console.log(`Printing log: ${statuslog}`);

            if (statuslog[0] !== 'E' && statuslog[0] !== 'C') {
                console.log("Setting gpc to 0");
                gpc = 0;
                res.json({ success: true, registers: { "x1": 0 }, memory: { "0x10000": 0 }, statuslog: statuslog });
            } else {
                for (let i = 1; i < lines.length; i++) {
                    if (lines[i] === '') {
                        isRegisters = false;
                        console.log('Memory starts at line', i);
                        continue;
                    }
                    if (isRegisters) {
                        registers[`x${registerCount}`] = lines[i];
                        registerCount++;
                    } else {
                        const address = `0x${(0x10000 + memCount).toString(16)}`;
                        memory[address] = lines[i];  // Map address to value
                        memCount++;
                    }
                }
                res.json({ success: true, registers: registers, memory: memory, statuslog: statuslog, gpc: gpc });
            }
        });

        simProcess.on('error', (error) => {
            console.error('Error executing simulator', error);
            res.status(500).json({ success: false, message: 'Simulator execution failed' });
        });

    } catch (err) {
        console.error('Error writing to file', err);
        res.status(500).json({ success: false, message: 'Internal Server Error' });
    }
});

app.get('/setzero', async (req, res) => {
    gpc = 0;
    res.json({ success: true, message: 'PC set to zero' });
});

app.listen(3000, '192.168.51.120', () => {
    console.log('Server running on port 3000');
});
