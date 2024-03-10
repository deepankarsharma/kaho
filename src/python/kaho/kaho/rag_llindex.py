from llama_index.core import SimpleDirectoryReader
from llama_index.core import VectorStoreIndex

documents = SimpleDirectoryReader("/Users/dman/rag_src").load_data()
vector_index = VectorStoreIndex.from_documents(documents)
vector_index.as_query_engine()