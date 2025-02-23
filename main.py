from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import PromptFewShot as Prompt

import logging

load_dotenv()

if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    gpt = OpenAIClient(logger=logger)
    oracle = Oracle(logger=logger)

    valid = False
    iteration = 0
    question = "implement a fibonacci sequence in C++ for the first 10 numbers"
    prompt = Prompt(question)
    
    while not valid and iteration < 10:
        iteration += 1
        logger.info(f"Iteration {iteration}")

        try:
            prompt_message = prompt.get_prompt()
            answer = gpt.interact(prompt_message)
            oracle.set_iteration_title(f"{prompt.title}_{iteration}")
            valid = oracle.validate_output(answer)

            if valid:
                logging.debug(f"Final answer: {answer}") 
                prompt.save_final_answer(answer)

        except Exception as e:
            print(str(e))
            prompt.save_intermediate_answer(answer, str(e))

        
    

