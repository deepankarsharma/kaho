import os
from langchain_community.document_loaders import DirectoryLoader
from langchain.text_splitter import CharacterTextSplitter
from langchain_community.vectorstores import FAISS
from langchain_openai import OpenAIEmbeddings
from langchain_community.llms import OpenAI
from langchain.chains import RetrievalQA

def load_documents(folder_path):
    # Load documents from the folder
    loader = DirectoryLoader(folder_path)
    documents = loader.load()

    # Split documents into chunks
    text_splitter = CharacterTextSplitter(chunk_size=1000, chunk_overlap=0)
    docs = text_splitter.split_documents(documents)

    return docs

def create_vector_store(documents, embeddings):
    # Create vector store
    vector_store = FAISS.from_documents(documents, embeddings)

    return vector_store

def query_vector_store(vector_store, query, llm):
    # Create retrieval QA chain
    qa = RetrievalQA.from_chain_type(llm=llm, chain_type="stuff", retriever=vector_store.as_retriever())

    # Query the vector store
    response = qa.run(query)

    return response

# Specify the folder path containing the documents
folder_path = '/Users/dman/rag_src'

# Specify the URL of your llama.cpp server's OpenAI-compatible endpoint
api_url = 'http://localhost:8080/v1'

# Load documents
documents = load_documents(folder_path)

# Initialize OpenAI embeddings with the llama.cpp server URL
embeddings = OpenAIEmbeddings(openai_api_base=api_url)

# Create vector store
vector_store = create_vector_store(documents, embeddings)

# Initialize OpenAI language model with the llama.cpp server URL
llm = OpenAI(openai_api_base=api_url)

# Perform RAG query
query = 'what servers exist in this codebase'
response = query_vector_store(vector_store, query, llm)

# Print the response
print(response)