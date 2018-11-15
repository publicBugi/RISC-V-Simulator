#!/bin/bash

find ./InstructionTests/ -maxdepth 1 -name *.bin -exec ./RISCVSIM {} \;