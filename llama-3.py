import transformers
import torch
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

model_id = "meta-llama/Meta-Llama-3-8B-Instruct"

tokenizer = transformers.AutoTokenizer.from_pretrained(model_id)

pipeline = transformers.pipeline(
    "text-generation",
    model=model_id,
    tokenizer=tokenizer,
    model_kwargs={"torch_dtype": torch.float32},
    device="cuda",
)

messages = [
    {"role": "system", "content": "You are my mental health assistant Tars. Limit responses to 2 or 3 sentences maximum.\
    For each response, categorize it into ONLY one of the following: dismissive, question, sad, happy, smiley. Do not select a\
    word not on the list. Put your choice as the first word before any response in brackets. For example, [sad] or [question]"},
]

class HTTPRequestHandler(BaseHTTPRequestHandler):

    def do_POST(self):
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length).decode('utf8')

        messages.append({"role": "user", "content": data})

        prompt = pipeline.tokenizer.apply_chat_template(
                messages,
                tokenize=False,
                add_generation_prompt=True
        )

        terminators = [
            pipeline.tokenizer.eos_token_id,
            pipeline.tokenizer.convert_tokens_to_ids("<|eot_id|>")
        ]

        outputs = pipeline(
            prompt,
            max_new_tokens=256,
            eos_token_id=terminators,
            do_sample=True,
            temperature=0.6,
            top_p=0.9,
        )

        response = outputs[0]["generated_text"][len(prompt):]
        response += "\n"

        messages.append({"role": "assistant", "content": response})

        self.send_response(200)
        self.end_headers()
        self.wfile.write(response.encode("utf-8"))

server = HTTPServer(('localhost', 8080), HTTPRequestHandler)

print("Started server")
server.serve_forever()
