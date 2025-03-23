# RoboSim State Machines Translation Using LLMs

This repository proposes an approach using LLM to translate notations of state machines in RoboSim - a UML-like notation designed specifically for simulation of autonomous and mobile robots - into C++ codes.
The framework integrates an LLM oracle using C++ compiler to validate the LLM's translation accuracy, addressing syntactical issues.

## Setup

This is the setup to run on ubuntu machines.  

### Pre-requisites

Follow the steps in each link to install the required packages:  

- (pyenv)[https://github.com/pyenv/pyenv] 
- (pipenv)[https://pipenv.pypa.io/en/latest/installation.html]  
- (libbost)[https://www.boost.org/]  
- (librcsc)[https://github.com/helios-base/librcsc]

### Running

1. Run `$ pipenv sync` to install required python packages.
2. Run `$ pipenv shell` to activate virtual environment.
3. Run `python main.py`. 