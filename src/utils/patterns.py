DEFAULT_JSON_PATTERN = {
  "type": "object",
  "properties": {
    "code": {
        "type": "string",
        "description": "C++ source code. The class name is 'StateMachine'."
    }
  },
  "required": ["code"],
  "additionalProperties": False
}