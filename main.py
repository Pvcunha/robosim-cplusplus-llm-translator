from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import PromptFewShot as Prompt
from pathlib import Path

import pandas as pd

import logging

load_dotenv()

if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    gpt = OpenAIClient(logger=logger)
    oracle = Oracle(logger=logger)

    question = "Translate the codes from Python to C++."
    csv = {}

    for input in Path('scr/dataset').iterdir():
        csv["Code ID"].append(input.name)
        iteration = 0

        with open(input, 'r', encoding='utf-8') as file:
            prompt = Prompt(question, file.read())

        for request in range(MAX_REQUEST):
            csv["Request ID"].append(request)
            valid = False
            
            while not valid and iteration < MAX_INTERACTIONS:
                iteration += 1
                logger.info(f"Iteration {iteration}")

                try:
                    prompt_message = prompt.get_prompt()
                    answer = gpt.interact(prompt_message)
                    # oracle.set_iteration_title(f"{prompt.title}_{iteration}")
                    # valid = oracle.validate_output(answer)
                    if interation == 2:
                        valid = True

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
        
    df = pd.DataFrame.from_dict(csv)
