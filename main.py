from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import Prompt as Prompt
from src.utils import *
from pathlib import Path
from time import sleep

from src import errors

import pandas as pd

import logging

load_dotenv()

if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s",
    )

    gpt = OpenAIClient(logger=logger)
    oracle = Oracle(logger=logger)

    csv= {
        "ID": [],
        "Result": [],
        "Loop Count": [],
        "Chat History": [],
        "Valid Answer": []
    }

    with open('res/few-shot.txt') as file: 
        question = file.read()
    
    for input in Path("src/dataset/robocin").glob("*.rst"):
        with open(input, "r", encoding="utf-8") as file:
            prompt = Prompt(question=question, code=file.read())

        logger.info(f"Processing {input.name}")
        for request in range(MAX_REQUEST_PER_FILE):
            csv["ID"].append(f"{input.name}_{request}")
            answer = None

            for retry in range(MAX_RETRY_PER_REQUEST):
                sleep(3)
                logger.info(f"Request {request} for retry {retry}")

                try:
                    print(prompt.get_prompt())
                    answer = gpt.interact(prompt.get_prompt())
                    oracle.set_iteration_title(f"{input.name}_{request}_{retry}")
                    valid = oracle.validate_output(answer)

                    if valid: 
                        logger.info(f"Valid response found: {answer}")
                        break

                except (errors.InvalidOutputFormatError,errors.CompileError) as e:
                    prompt.set_history(answer, str(e))
                    logger.info(f"Invalid response error: {str(e)}")

            csv["Result"].append(valid)
            csv["Loop Count"].append(retry)
            csv["Chat History"].append(prompt.get_messages())
            csv["Valid Answer"].append(answer)

    assert len(csv["ID"]) == len(csv["Result"]) == len(csv["Loop Count"]) == len(csv["Chat History"]) == len(csv["Valid Answer"])

    df = pd.DataFrame.from_dict(csv)
    df.to_csv("few-shot.csv", index=False)
