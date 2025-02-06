import subprocess
import os
import json
import typing
from src import errors



class Oracle:
    def _save_code(self, code):
        with open("./tmp/code.cpp", "w") as file:
            file.write(code)

    def _parse_output(self, ai_reponse) -> typing.Dict[str, str] :
        try:
            data = json.loads(ai_reponse)
            if "code" not in data:
                raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_reponse))
        except Exception as e:
            raise errors.InvalidOutputFormatError("Invalid output format: {}".format(ai_reponse))

        return data

    
    def _compile_and_run_cpp(self, source_file):
        executable = "output.exe" if os.name == "nt" else "./output"

        # Compile the C++ code using g++
        compile_command = ["g++", source_file, "-o", executable]
        compile_process = subprocess.run(compile_command, capture_output=True, text=True)

        if compile_process.returncode != 0:
            print("Compilation failed:")
            print(compile_process.stderr)
            raise errors.CompilationError(compile_process.stderr)

        print("Compilation successful. Running the program...\n")

        # Run the compiled executable
        run_process = subprocess.run([executable], capture_output=True, text=True)
        print(run_process.stdout)
    
    def validate_output(self, output) -> bool:
        data = self._parse_output(output)
        self._save_code(data["code"])

        self._compile_and_run_cpp("./tmp/code.cpp")
        return True
