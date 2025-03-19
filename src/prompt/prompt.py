from typing import List

class Prompt():
    def __init__(self, question: str, code: str) -> None:
        self.main_question = f"{question}\n{code}"
        self.final_answer = None
        self.messages = [{"role": "user", "content": self.main_question}]

    def save_final_answer(self, answer) -> None:
        self.final_answer = answer

    def add_output(self,output):
        self.messages.append({"role": "assistant", "content": output})

    def add_output_error(self,outputError):
        self.messages.append({"role": "user", "content": outputError})

    def get_messages(self) -> str:
        return self.messages

    def get_final_answer(self) -> str | None:
        return self.final_answer