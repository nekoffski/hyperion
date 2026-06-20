#!/bin/bash

if ! [ -d ./venv ]; then
    echo "Virtual environment not found. Please run ./tests-integration/env.sh first."
    exit 1
fi

source ./venv/bin/activate
PYTHON_SRC=./tests-integration pytest ./tests-integration/test_*.py $@
