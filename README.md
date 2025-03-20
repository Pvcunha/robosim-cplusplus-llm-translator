# RoboSim State Machines Translation Using LLMs

## 📚 Overview
This repository presents a framework that uses Large Language Models (LLMs) to automatically translate state machine descriptions written in **RoboSim** — a UML-like notation designed for simulating autonomous and mobile robots — into **C++ code**.

The generated code follows the structure of Boost Statechart and uses the **librcsc** library, widely used in **RoboCup 2D** soccer simulations.

## ✅ Key Features
- Automatic translation of RoboSim state machines into C++.
- Uses **OpenAI GPT models** as the LLM backend.
- Iterative prompting and automatic re-tries until a valid solution is reached or max attempts are exhausted.
- Export of results and history in CSV format for analysis.

## 📂 Project Structure
```
├── assets/
│   └── example.txt            # Example prompt for guiding the LLM
├── src/
│   ├── clients.py             # Wrapper for OpenAI API interaction
│   ├── oracle.py              # Validation and iteration control logic
│   ├── prompt.py              # Prompt template and formatting
│   ├── utils.py               # Utility functions
│   └── dataset/test/          # Folder containing input RoboSim files
├── main.py                    # Main execution script
├── output.csv                 # Output generated with validation results
└── README.md                  # Project documentation
```

## ▶️ How to Run
1. Clone this repository:
```bash
git clone <https://github.com/Pvcunha/robosim-cplusplus-llm-translator.git>
cd <robosim-cplusplus-llm-translator>
```

2. Create a virtual environment using **Pipenv**:
```bash
pip install pipenv
pipenv install
pipenv shell
```

3. Set up environment variables in a `.env` file:
```
OPENAI_API_KEY=your_api_key_here
```

4. Download and build the **librcsc** library from the official repository:
```
git clone https://github.com/helios-base/librcsc.git
cd librcsc
mkdir build && cd build
cmake ..
make
sudo make install
```

5. Download and install the **Boost Statechart** library from the official Boost website:
```
https://www.boost.org/doc/libs/1_78_0/libs/statechart/doc/index.html
```

6. Run the translation process:
```bash
python main.py
```

## ⚙️ Environment Requirements
- Python 3.10+
- OpenAI API key
- librcsc (https://github.com/helios-base/librcsc)
- Boost Statechart library (https://www.boost.org/doc/libs/1_78_0/libs/statechart/doc/index.html)


