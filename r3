#! /bin/bash

FREQ=5000
N_CNX=1000000



# Set up environment -----------------------------------------

# Change these three to personalize for your system.

INSTALL_ROOT=/home/mick/latest
PROTON_INSTALL_DIR=${INSTALL_ROOT}/qpid-proton/install
DISPATCH_DIR=${INSTALL_ROOT}/skupper-router
DISPATCH_INSTALL_DIR=${DISPATCH_DIR}/install

export LD_LIBRARY_PATH=${DISPATCH_INSTALL_DIR}/lib:${PROTON_INSTALL_DIR}/lib64
export PYTHONPATH=${DISPATCH_INSTALL_DIR}/lib/skupper-router/python:${DISPATCH_INSTALL_DIR}/lib/python3.11/site-packages:${DISPATCH_DIR}/python:${INSTALL_ROOT}/qpid-proton/install/lib64:${INSTALL_ROOT}/qpid-proton/install/include/proton
echo "PYTHONPATH == ${PYTHONPATH}"

ROUTER=${DISPATCH_INSTALL_DIR}/sbin/skrouterd


SERVER=./server
CLIENT=./client


# Start the router ------------------------------

echo "${ROUTER} --config ./skrouterd.conf"
${ROUTER} --config ./skrouterd.conf  > ./router_output 2>&1  & 
ROUTER_PID=$!
echo "Router has started with PID ${ROUTER_PID}"

echo "sleeping for 15 seconds to let you start 'top' on ${ROUTER_PID}"
sleep 15

${SERVER} 5802 ${N_CNX} ${FREQ} &
SERVER_1_PID=$!
echo "Server 1 has started with PID ${SERVER_1_PID}"

${SERVER} 5804 ${N_CNX} ${FREQ} &
SERVER_2_PID=$!
echo "Server 2 has started with PID ${SERVER_2_PID}"

${SERVER} 5806 ${N_CNX} ${FREQ} &
SERVER_3_PID=$!
echo "Server 3 has started with PID ${SERVER_3_PID}"

sleep 3

${CLIENT} 5801 ${N_CNX} ${FREQ} &
CLIENT_1_PID=$!
echo "Client 1 has started with PID ${CLIENT_1_PID}"

${CLIENT} 5803 ${N_CNX} ${FREQ} &
CLIENT_2_PID=$!
echo "Client 2 has started with PID ${CLIENT_2_PID}"

${CLIENT} 5805 ${N_CNX} ${FREQ} &
CLIENT_3_PID=$!
echo "Client 3 has started with PID ${CLIENT_3_PID}"


echo "Waiting for servers to quit..."
wait ${SERVER_1_PID}
wait ${SERVER_2_PID}
wait ${SERVER_3_PID}
echo "Waiting for clients to quit..."
wait ${CLIENT_1_PID}
wait ${CLIENT_2_PID}
wait ${CLIENT_3_PID}

echo "Shutting down router..."
kill -15 ${ROUTER_PID}
wait ${ROUTER_PID}

echo "Done."




