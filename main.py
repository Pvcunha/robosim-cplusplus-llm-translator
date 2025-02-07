from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient

import logging

load_dotenv()

if __name__ == "__main__":
    gpt = OpenAIClient()
    oracle = Oracle()

    logger = logging.getLogger(__name__)
    logging.basicConfig(level=logging.DEBUG)

    try:
        data = gpt.get_hello_world_cpp()
        print(oracle.validate_output(data))
    except Exception as e:
        print(str(e))