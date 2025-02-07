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

class RequestFailedError(Exception):
    def __init__(self, message):
        self.message = f"The request failed. Please try again. {message}"
        super().__init__(self.message)