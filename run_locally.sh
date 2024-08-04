#!/bin/sh
BINARY_HOME=./build
INPUT_HOME=./input
INPUT=${INPUT_HOME}/AB_NYC_2019.csv

cat ${INPUT} | ${BINARY_HOME}/mapper | ${BINARY_HOME}/reducer_avg > avg
cat avg

cat ${INPUT} | ${BINARY_HOME}/mapper | ${BINARY_HOME}/reducer_disp avg > dispersion
cat dispersion
