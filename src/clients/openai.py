from openai import OpenAI
from src import errors
import re

class OpenAIClient:
    def __init__(self):
        self.client = OpenAI()
        self.default_output_format = '{"code": "#include <iostream>\n\nint main() {\n    std::cout << \"Hello, World!\" << std::endl;\n    return 0;\n}"}'
        self.model = "gpt-4"
        self.messages = [] # to append messages and generate an message history between the user and the AI
        self.regex = r'\{.*\}'

    def interact(self, message: str):
        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=[{"role": "user", "content": message}]
            )

            return response.choices[0].message.content
        except Exception as e:
            return str(e)

    def get_hello_world_cpp(self):
        try:
            response = self.client.chat.completions.create(
                model="gpt-4",
                messages=[{"role": "user", "content": f"Your output must be a json in this format {self.default_output_format}. Write a 'Hello, World!' program in C++."}]
            )

            return self._parse_response(response.choices[0].message.content)
        except Exception as e:
            return str(e)
    
    def _parse_response(self, response):
        match = re.search(self.regex, response)
        if match:
            print(match)
            return match.group(0)

        raise errors.InvalidResponseError("Invalid response: {}".format(response))
        