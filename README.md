# tars
Robot chat therapist

## Running (After compiling)
`python3 llama-3.py`

`./command -m ./models/ggml-base.en.bin -ac 768`

### On the Pi
`sudo ./animation 8 --led-rows=32 --led-cols=64`

`sudo python3 animation-server.py`

## Open Source Credit
https://huggingface.co/meta-llama/Meta-Llama-3-8B-Instruct

https://github.com/rhasspy/piper

https://github.com/hzeller/rpi-rgb-led-matrix

https://github.com/datacratic/curlpp

https://github.com/ggerganov/whisper.cpp
