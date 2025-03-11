from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import PromptFewShot as Prompt
from src.utils import *
from pathlib import Path
from time import sleep

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
    # Question  
    You will be given a RoboSim state machine. Can you please translate to an equivalent C++ code using the Boost Statechart Library?
    
    ## Rules
        - server::Vector2D is an alias for rcsc::Vector2D data type
    
    ## Instructions:
        - Remember to use librcsc from RoboCup 2D
        - Keep the state machine structure and transitions
        - Keep states and transitions logical behavior
        - Remember to add the conditional bodies in the transitions
        - Ensure that the machine execution only ends when the terminal state is reached (use the "terminate" method)
        - Think carefully, this is important.
    '''

    csv= {
        "ID": [],
        "Result": [],
        "Loop Count": [],
        "Chat History": [],
        "Valid Answer": []
    }

    for input in Path("src/dataset/test").iterdir():

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
            iteration = 0
            while not valid and iteration < MAX_INTERACTIONS:
                iteration += 1
                logger.info(f"Iteration {iteration} request {request}")

                prompt_message = prompt.get_prompt()
                global answer
                answer = ""
                try:
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
