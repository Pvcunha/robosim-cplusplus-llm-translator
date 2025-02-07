from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient

import logging

load_dotenv()

if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    logging.basicConfig(level=logging.INFO)

    gpt = OpenAIClient(logger=logger)
    oracle = Oracle(logger=logger)

    try:
        data = gpt.get_hello_world_cpp()
        print(data)
        logger.debug(oracle.validate_output(data))
    except Exception as e:
        print(str(e))