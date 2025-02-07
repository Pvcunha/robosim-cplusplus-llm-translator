from openai import OpenAI
from src import errors
from src.utils import DEFAULT_JSON_PATTERN
import logging

class OpenAIClient:
    def __init__(self, logger: logging.Logger=None):
        self.client = OpenAI()
        self.default_output_format = '{"code": "#include <iostream>\n\nint main() {\n    std::cout << \"Hello, World!\" << std::endl;\n    return 0;\n}"}'
        self.model = "gpt-4"
        self.messages = [] # to append messages and generate an message history between the user and the AI
        self.logger = logger
        logging.addLevelName(1, "OpenAIClient")
    def interact(self, message: str):
        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=[{"role": "user", "content": message}]
            )

            return response.choices[0].message.content
        except Exception as e:
            raise errors.RequestFailedError(str(e))

    def get_hello_world_cpp(self):
        try:
            response = self.client.chat.completions.create(
                model="gpt-4",
                messages=[
                    {"role": "user", "content": "Write a C++ Hello World program"}
                ],
                tools=[{
                    "type": "function",
                    "function": {
                        "name": "generate_code",
                        "description": "Generate C++ source code in JSON format.",
                        "parameters": DEFAULT_JSON_PATTERN,
                        "strict": True,
                    },
                }],
                tool_choice="required"
            )
            
            args = response.choices[0].message.tool_calls[0].function.arguments
            
            return args
        except Exception as e:
            raise errors.RequestFailedError(str(e))
        