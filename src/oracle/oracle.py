import subprocess
import os
import json
import typing
import logging

from src import errors


class Oracle:
    def __init__(self, logger: logging.Logger=None):
        self.logger = logger
        self.regex = r'\{.*\}'
        self.iteration_title = "Iteration"

    def _save_code(self, code):
        with open(f"./tmp/{self.iteration_title}.cpp", "w") as file:
            file.write(code)

    def _parse_json(self, ai_json) -> typing.Dict[str, str] :
        try:
            self.logger.debug("Parsing JSON")
            data = json.loads(ai_json)
            if "code" not in data:
                raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_json))
        except Exception as e:
            raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_json))

        return data
    
    def _compile_and_run_cpp(self, source_file):
        executable = "output.exe" if os.name == "nt" else "./output"

        # Compile the C++ code using g++
        compile_command = ["g++", source_file, "-o", executable]
        compile_process = subprocess.run(compile_command, capture_output=True, text=True)

        if compile_process.returncode != 0:
            self.logger.error(f"Compilation failed. {compile_process.stderr}")
            raise errors.CompilationError(compile_process.stderr)

        self.logger.info("Compilation successful. Running the program...\n")

        # Run the compiled executable
        run_process = subprocess.run([executable], capture_output=True, text=True)
        self.logger.info(f"Program output:\n{run_process.stdout}")
        
    def validate_output(self, ai_response) -> bool:

        self.logger.info("Validating the output...")

        self.logger.debug("AI response: {}".format(ai_response))

        data = self._parse_json(ai_response)

        self._save_code(data["code"])

        self._compile_and_run_cpp("./tmp/code.cpp")
        
        return True

    def set_iteration_title(self, title):
        self.iteration_title = title
