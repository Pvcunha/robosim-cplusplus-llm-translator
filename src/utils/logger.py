import logging

class Logger:
    def __init__(self, level=logging.INFO):
        logging.basicConfig(level=level)
        self.logger = logging.getLogger()
        
    def info(self, message):
        self.logger.info(message)

    def debug(self, message):
        self.logger.debug(message)

    def error(self, message):
        self.logger.error(message)
    
    @staticmethod
    def getLogger(level=logging.INFO):
        return Logger(level)