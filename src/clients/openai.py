import openai
import logging
import time
from openai import OpenAI
from src import errors
from src.utils import DEFAULT_JSON_PATTERN, MAX_RETRY_PER_REQUEST, RETRY_DELAY

class OpenAIClient:
    def __init__(self, logger: logging.Logger = None):
        self.client = OpenAI()
        self.default_output_format = '{"code": "#include <iostream>\n\nint main() {\n    std::cout << "Hello, World!" << std::endl;\n    return 0;\n}"}'
        self.messages = (
            []
        )  # to append messages and generate an message history between the user and the AI
        self.logger = logger

    def interact(self, message: str):
        try:
            response = self.client.chat.completions.create(
                model="gpt-4o",
                messages=[{"role": "user", "content": message}],
                
                tools=[
                    {
                        "type": "function",
                        "function": {
                            "name": "generate_code",
                            "description": "Generate C++ source code in JSON format.",
                            "parameters": DEFAULT_JSON_PATTERN,
                            "strict": True,
                        },
                    }
                ],
                tool_choice="required",
                temperature=0.6,
                top_p=0.7,
            )
            
            args = response.choices[0].message.tool_calls[0].function.arguments

            return args
        except openai.RateLimitError as e:
            self.logger.error("Rate limit exceeded")
            time.sleep(RETRY_DELAY)
        except Exception as e:
            raise errors.RequestFailedError(str(e))