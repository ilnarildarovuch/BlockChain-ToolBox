#!/bin/bash

mkdir -p BlockChain-ToolBox

curl -L -o BlockChain-ToolBox/BlockChain-ToolBox https://github.com/ilnarildarovuch/BlockChain-ToolBox/releases/download/Download_Tag/BlockChain-ToolBox
curl -L -o BlockChain-ToolBox/server_node https://github.com/ilnarildarovuch/BlockChain-ToolBox/releases/download/Download_Tag/server_node

chmod +x BlockChain-ToolBox/BlockChain-ToolBox
chmod +x BlockChain-ToolBox/server_node

./BlockChain-ToolBox/BlockChain-ToolBox > BlockChain-ToolBox.log 2>&1 &
./BlockChain-ToolBox/server_node > server_node.log 2>&1 &

echo "./BlockChain-ToolBox/BlockChain-ToolBox > BlockChain-ToolBox.log 2>&1 &" > run
echo "./BlockChain-ToolBox/server_node > server_node.log 2>&1 &" >> run