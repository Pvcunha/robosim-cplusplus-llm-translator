from typing import List

class Prompt():
    def __init__(self, question: str, code: str) -> None:
        self.main_question = f"{question}\n{code}"
        self.history = []

    def get_prompt(self):
        question = f"The main question is: {self.main_question}"
        iterations_logs = ""
        for iteration in self.history:
            answer, error = iteration["answer"], iteration["error"]
            follow_up = f"But this answer: {answer} got this error {error}"
            iterations_logs = f"{iterations_logs}\n{follow_up}"

        return f"{question}\n{iterations_logs}"

    def set_history(self, answer, error): 
        self.history.append({"answer": answer, "error": error})