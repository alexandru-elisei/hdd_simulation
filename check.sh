#!/bin/bash

make clean
make

if [ $? -eq 0 ]; then
	for i in {1..13}; do
		echo ""
		./myHDD "$i.in" out
		diff out "$i.out"
		if [ $? -eq 0 ]; then
			echo "[$i] --  OK!  --"
		else
			echo "[$i] -- ERROR --"
		fi
		echo ""
	done

	for i in {1..6}; do
		echo ""
		./myHDD "e$i" out
		diff out "e$i.out"
		if [ $? -eq 0 ]; then
			echo "[e$i] --  OK!  --"
		else
			echo "[e$i] -- ERROR --"
		fi
		echo ""
	done

	make clean

fi
