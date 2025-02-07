import subprocess
import os
import json
import typing
import re
import logging

from src import errors


class Oracle:
    def __init__(self, logger=None):
        self.logger = logger
        self.logger.addLevelName(1, "ORACLE")
        self.regex = r'\{.*\}'

    def _save_code(self, code):
        with open("./tmp/code.cpp", "w") as file:
            file.write(code)

    def _parse_json(self, ai_json) -> typing.Dict[str, str] :
        try:
            data = json.loads(ai_json)
            if "code" not in data:
                raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_json))
        except Exception as e:
            raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_json))

        return data

    def _parse_response(self, ai_response):
        match = re.search(self.regex, ai_response, re.DOTALL)
        if match:
            print(match)
            return match.group(0)

        raise errors.InvalidResponseError("Invalid response: {}".format(ai_response))
    
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
        print(run_process.stdout)
        
    def validate_output(self, ai_response) -> bool:
        self.logger.info("Validating the output...")
        self.logger.debug("AI response: {}".format(ai_response))
        json = self._parse_response(ai_response)
        self.logger.debug("Parsed JSON: {}".format(json))
        #data = self._parse_json(json)

        #self._save_code(data["code"])

       # self._compile_and_run_cpp("./tmp/code.cpp")
        return True
