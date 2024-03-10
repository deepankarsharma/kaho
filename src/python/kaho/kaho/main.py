from fastapi import FastAPI
from pydantic import BaseModel
import os


os.environ['OPENAI_API_KEY'] = '<UNSET>'

app = FastAPI()

class HelloRequest(BaseModel):
    name: str

class HelloResponse(BaseModel):
    message: str

@app.get("/")
def read_root():
    return {"Hello": "World"}

@app.post("/hello", response_model=HelloResponse)
def hello(request: HelloRequest):
    return HelloResponse(message=f"Hello, {request.name}!")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
