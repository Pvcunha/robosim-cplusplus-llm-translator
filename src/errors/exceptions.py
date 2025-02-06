class CompileError(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

class InvalidOutputFormatError(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

class InvalidResponseError(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)