const express = require('express');
const cors = require('cors');
const { promises: fs } = require('fs');
const { spawn } = require('child_process');
const http = require('http');
const { Server } = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = new Server(server, { cors: { origin: '*' } });

app.use(cors());
app.use(express.static('./public'));

io.on('connection', (socket) => {
    console.log('A client connected');
    socket.gpc = 0;

    socket.on('getData', async ({ code, arg, pc, cacheConfig }) => {
        console.log('Received arg:', arg);
        console.log('Received pc:', pc);
        console.log('Received cacheConfig:', cacheConfig);
        socket.gpc = pc;
        try {
            // Save input.s
            await fs.writeFile('./Simulator/input.s', code);
            console.log('Code File written successfully');
            
            // Save cacheConfig.txt
            let fileContent = '';
            for (const [key, value] of Object.entries(cacheConfig)) {
                fileContent += `${value}\n`;
                console.log(`key:${key} value:${value}`);
            }

            console.log(`cacheConfig:\n${fileContent}`);
            await fs.writeFile('./Simulator/cacheConfig.txt', fileContent);
            console.log('cacheConfig written successfully');
            
            // Run simulator
            const simProcess = spawn('./riscv_sim', [arg, socket.gpc], { cwd: './Simulator' });
            console.log(`./riscv_sim ${arg} ${socket.gpc}`);

            let dataOutput = '';
            
            // Handle step/run update
            if (arg === 'step') {
                console.log('checking for step');
                socket.gpc += 4;
            }

            if (arg === 'run') {
                console.log('checking for run');
                socket.gpc = 0;
            }

            simProcess.stdout.on('data', (data) => {
                dataOutput += data;
            });

            simProcess.on('close', () => {
                const lines = dataOutput.split('\n');
                let registers = {};
                let memory = {};
                let statuslog = '';
                let isRegisters = true;
                let registerCount = 0;
                let memCount = 0;
                let hits = 0;
                let misses = 0;

                statuslog = lines[0];
                console.log(`Printing log: ${statuslog}`);

                if (statuslog[0] !== 'E' && statuslog[0] !== 'C') {
                    console.log('Setting gpc to 0');
                    socket.gpc = 0;
                    socket.emit('response', {
                        success: true,
                        registers: { x1: 0 },
                        memory: { '0x10000': 0 },
                        statuslog: statuslog,
                        gpc: socket.gpc,
                        hits: hits,
                        misses: misses,
                    });
                } else {
                    for (let i = 1; i < lines.length; i++) {
                        if (lines[i] === '') {
                            isRegisters = false;
                            continue;
                        }
                        if (isRegisters) {
                            registers[`x${registerCount}`] = lines[i];
                            registerCount++;
                        } else if (memCount < 1023) {
                            const address = `0x${(0x10000 + memCount).toString(16)}`;
                            memory[address] = lines[i];
                            memCount++;
                        } else {
                            hits = lines[i];
                            misses = lines[i + 1];
                            break;
                        }
                    }
                    console.log('hits:', hits);
                    console.log('misses:', misses);
                    console.log('Emitting response');
                    console.log('gpc is', socket.gpc);

                    socket.emit('response', {
                        success: true,
                        registers: registers,
                        memory: memory,
                        statuslog: statuslog,
                        gpc: socket.gpc,
                        hits: hits,
                        misses: misses,
                    });
                }
            });

            simProcess.on('error', (error) => {
                console.error('Error executing simulator', error);
                socket.emit('error', { success: false, message: 'Simulator execution failed' });
            });

        } catch (err) {
            console.error('Error during processing', err);
            socket.emit('error', { success: false, message: 'Internal Server Error' });
        }
    });

    socket.on('setzero', () => {
        socket.gpc = 0;
    });
    
    socket.on('disconnect', () => {
        console.log('A client disconnected');
    });
});

server.listen(3000, () => {
    console.log('Socket.IO server running on port 3000');
});
