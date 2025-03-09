from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import PromptFewShot as Prompt
from src.utils import *
from pathlib import Path

import pandas as pd

import logging

load_dotenv()

if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s",
        # filename="log.log",
        # filemode="w"
    )

    gpt = OpenAIClient(logger=logger)
    oracle = Oracle(logger=logger)

    question = '''
    Translate the following Robosim state machine code into an equivalent C++ implementation using the Boost Statechart Library. Ensure that:

    - The state machine will be placed within a class with the name "StateMachine".
    - The state machine structure and transitions remain the same.
    - The states and transitions preserve their logical behavior.
    - The final state correctly terminates execution.
    '''
    csv= {
        "ID": [],
        "Result": [],
        "Loop Count": [],
        "Chat History": [],
        "Valid Answer": []
    }

    for input in Path("src/dataset/test").iterdir():

        iteration = 0

        global prompt
        with open(input, "r", encoding="utf-8") as file:
            exampleFile = open("assets/example.txt", "r", encoding="utf-8")
            example = exampleFile.read()
            prompt = Prompt(title="foo", question=question, example=example, code=file.read())

        logger.info(f"Processing {input.name}")

        for request in range(MAX_REQUEST):
            # csv["Code ID"].append(input.name)
            # csv["Request ID"].append(request)
            csv["ID"].append(f"{input.name}_{request}")
            valid = False

            while not valid and iteration < MAX_INTERACTIONS:
                iteration += 1
                logger.info(f"Iteration {iteration} request {request}")

                try:
                    prompt_message = prompt.get_prompt()
                    answer = gpt.interact(prompt_message)
                    oracle.set_iteration_title(f"{input.name}_{request}_{iteration}")
                    valid = oracle.validate_output(answer)

                    if valid:
                        logger.debug(f"Final answer: {answer}")
                        prompt.save_final_answer(answer)

                except Exception as e:
                    logger.debug(str(e))
                    prompt.save_intermediate_answer(answer, str(e))

            csv["Result"].append(valid)
            csv["Loop Count"].append(iteration)
            csv["Chat History"].append(prompt.get_answers())
            csv["Valid Answer"].append(prompt.get_final_answer())

    assert len(csv["ID"]) == len(csv["Result"]) == len(csv["Loop Count"]) == len(csv["Chat History"]) == len(csv["Valid Answer"])

    df = pd.DataFrame.from_dict(csv)
    df.to_csv("output.csv", index=False)
