1. Find a open port
2. Allow user to download a model
3. Add checksums

Add macdeployqt

Add models
https://huggingface.co/mistralai/Mistral-7B-Instruct-v0.1
https://huggingface.co/stabilityai/stable-diffusion-xl-base-1.0
runpod integration

sudo apt-get install libxcb1-dev libxcb-util0-dev libxcb-cursor-dev libxcb-xv0-dev

sudo apt-get install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev

Things to do between now and mac launch


Today
1. Build where Python framework is in the executable
2. Figure out where source code will live and write a simple fastapi server
3. Write a simple fastapi server and launch that from a button
4. 
1. Add legal page


curl --request POST \
--url http://localhost:8080/completion \
--header "Content-Type: application/json" \
--data '{"prompt": "Building a website can be done in 10 simple steps:","n_predict": 128}'

curl http://localhost:8080/v1/embeddings \
-H "Content-Type: application/json" \
-d '{"input": "Deep is a good boy"}'