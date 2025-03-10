from abc import ABC, abstractmethod
from typing import List


class PromptInterface(ABC):
    @abstractmethod
    def get_prompt(self) -> str:
        pass

    @abstractmethod
    def save_intermediate_answer(self, answer, error) -> None:
        pass

    @abstractmethod
    def save_final_answer(self, answer) -> None:
        pass

# there is no example in this prompt
class PromptZeroShot(PromptInterface):
    def __init__(self, title: str, question: str, code: str) -> None:
        self.title = title
        self.main_question = f"{question}\n# Input Robosim code:\n{code}"
        self.answers = []
        self.final_answer = None

    def save_intermediate_answer(self, answer, error) -> None:
        self.answers.append({"answer": answer, "error": error})

    def save_final_answer(self, answer) -> None:
        self.save_intermediate_answer(answer, None)
        self.final_answer = answer

    def get_prompt(self) -> str:
        question = f"question: {self.main_question}"
        iterations_logs = ""
        for iteration in self.answers:
            answer, error = iteration["answer"], iteration["error"]
            follow_up = f"this answer: {answer} got this error {error}"
            iterations_logs = f"{iterations_logs}\n{follow_up}"

        prompt = f"{question}\n{iterations_logs}"
        return prompt

    def get_answers(self) -> List:
        return self.answers

    def get_final_answer(self) -> str | None:
        return self.final_answer

# there is an example in this prompt
class PromptFewShot(PromptInterface):
    def __init__(self, title: str, question: str, code: str, example: str) -> None:
        self.title = title
        self.main_question = f"{question}\n#Input RoboSim code:\n{code}\n # Implementation Example:\n{example}"
        self.answers = []
        self.final_answer = None

    def save_intermediate_answer(self, answer, error) -> None:
        self.answers.append({"answer": answer, "error": error})

    def save_final_answer(self, answer) -> None:
        self.save_intermediate_answer(answer, None)
        self.final_answer = answer

    def get_prompt(self) -> str:
        question = f"question: {self.main_question}"
        iterations_logs = ""
        for iteration in self.answers:
            answer, error = iteration["answer"], iteration["error"]
            follow_up = f"this answer: {answer} got this error {error}"
            iterations_logs = f"{iterations_logs}\n{follow_up}"

        prompt = f"{question}\n{iterations_logs}"
        return prompt

    def get_answers(self) -> List:
        return self.answers

    def get_final_answer(self) -> str | None:
        return self.final_answer
