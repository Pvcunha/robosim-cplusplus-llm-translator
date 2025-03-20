from dotenv import load_dotenv
from src.oracle import Oracle
from src.clients import OpenAIClient
from src.prompt import PromptZeroShot as Prompt
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
        - use just this imports:
            #include "rcsc/geom/vector_2d.h"
            #include <boost/statechart/custom_reaction.hpp>
            #include <boost/statechart/termination.hpp>

            #include <boost/statechart/event.hpp>
            #include <boost/statechart/result.hpp>
            #include <boost/statechart/simple_state.hpp>
            #include <boost/statechart/state.hpp>
            #include <boost/statechart/state_machine.hpp>
            #include <boost/statechart/transition.hpp>
            #include <rcsc/player/soccer_action.h>
            #include <rcsc/player/world_model.h>
            #include <rcsc/player/player_agent.h>


    ## Instructions:
        - Remember to use librcsc from RoboCup 2D
        - librcsc imports follows the pattern: #include <rcsc/...>
        - Keep the state machine structure and transitions
        - use boost statechart library
        - Keep states and transitions logical behavior
        - Remember to add the conditional bodies in the transitions
        - Ensure that the machine execution only ends when the terminal state is reached (use the "terminate" method)
        - Think carefully, this is important.
    '''

    question2 = '''
        ## Context
        Let RoboSim be a UML-like notation designed specifically for simulation of autonomous and mobile robots, and including timed primitives.
        Let RoboCup 2D be a category of autonomous multi-agent robots of a simulated soccer game.

        ## Instructions
        I will give you a Robosim state machine and your task is generate the equivalent C++ code using the Boost Statechart Library while keeping all state machine structure, transitions, and logic.

        DO NOT apply a direct translation. 

        Think carefully, give me a C++ code! Reason a little before answering, take your time, this is important!!!
        
        ## Rules
        - MUST CHANGE server::Vector2D to rcsc::Vector2D
        - MUST CHANGE server::PlayerAgent to rcsc::PlayerAgent 
        - MUST use rcsc::PlayerAgent as pointer
        - MUST add the conditional bodies in the transitions
        - MUST include function parameters in functions
        - MUST use just this imports:
            #include "rcsc/geom/vector_2d.h"
            #include <boost/statechart/custom_reaction.hpp>
            #include <boost/statechart/termination.hpp>

            #include <boost/statechart/event.hpp>
            #include <boost/statechart/result.hpp>
            #include <boost/statechart/simple_state.hpp>
            #include <boost/statechart/state.hpp>
            #include <boost/statechart/state_machine.hpp>
            #include <boost/statechart/transition.hpp>
            #include <rcsc/player/soccer_action.h>
            #include <rcsc/player/world_model.h>
            #include <rcsc/player/player_agent.h>

        {}

        {}

        ## Requested State Machine
        Here follows the RoboSim state machine about the goalie and kicker behavior, give me the equivalent C++ code, using Boost.Statechart C++ library and librcsc C++ library from RoboCup 2D.
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
            exampleFile = open("assets/example2.txt", "r", encoding="utf-8")
            # import pdb; pdb.set_trace()
            example = exampleFile.read()
            question = question2.format(example)
            prompt = Prompt(title="foo", question=question, code=file.read())

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
