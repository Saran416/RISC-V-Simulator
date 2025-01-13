const express = require('express');
const cors = require('cors');
const { promises: fs } = require('fs'); // Use the promise-based version of fs
const { spawn } = require('child_process');
const http = require('http');
const { Server } = require('socket.io');

const app = express();
const server = http.createServer(app); // Create HTTP server
const io = new Server(server, { cors: { origin: '*' } }); // Attach Socket.IO and enable CORS

app.use(cors());
app.use(express.static('./public')); // Serve static files if needed

io.on('connection', (socket) => {
    console.log('A client connected');

    // Handle "getData" event
    socket.on('getData', async ({ code, arg, pc }) => {
        console.log('Received arg:', arg);
        console.log('Received pc:', pc);

        let gpc = pc;

        try {
            // Write the code to a file asynchronously
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

            // Collect output data from the simulator
            simProcess.stdout.on('data', (data) => {
                dataOutput += data;
            });

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
                    socket.emit('response', {
                        success: true,
                        registers: { "x1": 0 },
                        memory: { "0x10000": 0 },
                        statuslog: statuslog
                    });
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
                            memory[address] = lines[i]; // Map address to value
                            memCount++;
                        }
                    }
                    socket.emit('response', {
                        success: true,
                        registers: registers,
                        memory: memory,
                        statuslog: statuslog,
                        gpc: gpc
                    });
                }
            });

            simProcess.on('error', (error) => {
                console.error('Error executing simulator', error);
                socket.emit('error', { success: false, message: 'Simulator execution failed' });
            });

        } catch (err) {
            console.error('Error writing to file', err);
            socket.emit('error', { success: false, message: 'Internal Server Error' });
        }
    });

    // Handle "setzero" event
    socket.on('setzero', () => {
        gpc = 0;
        socket.emit('response', { success: true, message: 'PC set to zero' });
    });

    // Handle "hexInstructions" event
    socket.on('hexInstructions', async ({ code }) => {
        console.log('Received code:', code);

        try {
            await fs.writeFile('./Simulator/input.s', code);
            console.log('File written successfully');

            const simProcess = spawn('./riscv_asm', { cwd: './Simulator' });
            console.log(`./riscv_asm`);

            let dataOutput = "";

            simProcess.stdout.on('data', (data) => {
                dataOutput += data;
            });

            simProcess.on('close', () => {
                const lines = dataOutput.split('\n');
                let hexInstructions = [];

                for (let i = 0; i < lines.length; i++) {
                    hexInstructions.push(lines[i]);
                }

                socket.emit('response', { success: true, hexInstructions: hexInstructions });
            });

            simProcess.on('error', (error) => {
                console.error('Error executing simulator', error);
                socket.emit('error', { success: false, message: 'Simulator execution failed' });
            });

        } catch (err) {
            console.error('Error writing to file', err);
            socket.emit('error', { success: false, message: 'Internal Server Error' });
        }
    });

    socket.on('disconnect', () => {
        console.log('A client disconnected');
    });
});

// Start the server
server.listen(3000, () => {
    console.log('Socket.IO server running on port 3000');
});
