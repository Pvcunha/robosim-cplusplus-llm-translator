import openai
import logging
import time
from src import errors
from src.utils import DEFAULT_JSON_PATTERN

class OpenAIClient:
    def __init__(self, logger: logging.Logger = None):
        self.client = openai.OpenAI()
        self.default_output_format = '{"code": "#include <iostream>\n\nint main() {\n    std::cout << "Hello, World!" << std::endl;\n    return 0;\n}"}'
        self.model = "gpt-4.5-preview"
        self.messages = (
            []
        )  # to append messages and generate an message history between the user and the AI
        self.logger = logger

    def interact(self, message: str):
        try:
            response = self.client.chat.completions.create(
                model=self.model,
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
                top_p=0.7,
                temperature=0.6
            )
            
            args = response.choices[0].message.tool_calls[0].function.arguments

            return args
        except openai.error.RateLimitError as e:
            self.logger.error("Rate limit exceeded")
        except Exception as e:
            raise errors.RequestFailedError(str(e))