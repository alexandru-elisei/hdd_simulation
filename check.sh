#!/bin/bash

ZERO=0

SCOREQT=0
SCOREST=0
SCOREBT=0

make clean
if test $? -ne 0; then
	echo "Makefile necorespunzator!"
	exit 1
fi
make build
if test $? -ne 0; then
	echo "Makefile necorespunzator!"
	exit 1
fi

EXEC=myHDD

for EX in {1..4}
do
	SCOREQ=0
	IN_FILE="tests/q_"$EX".in"
	USER_FILE="data.out"
	OUT_FILE="tests/q_"$EX".out"

	./$EXEC $IN_FILE $USER_FILE &> /dev/null
	
#penalizare daca are fisierul prea mare
	user_lines=$(wc -l < $USER_FILE)
	out_lines=$(wc -l < $OUT_FILE)

	
	if [[ $user_lines -gt $out_lines ]]
	then
		echo "QUEUE-test_$EX : -5 Fisierul de output prea mare"
		echo
		SCOREQ=$((SCOREQ-5))
	fi

#mergem pe numarul de linii al iesirii temei
	while read -r line_user && read -r line_out <&3; 
	do
                if [ "$line_user" == "$line_out" ]
                then

			SCOREQ=$((SCOREQ+1))
		else
			echo $line_user
		fi
	done < $USER_FILE 3<$OUT_FILE

	rm -f $USER_FILE

	if (( $SCOREQ < 0 ))
	then
        	SCOREQ=0
	fi
	
	echo "QUEUE-test_$EX .................... $SCOREQ/10"
        SCOREQT=$((SCOREQT + SCOREQ))
done

for EX in {1..5}
do
	SCORES=0
	IN_FILE="tests/s_"$EX".in"
	USER_FILE="data.out"
	OUT_FILE="tests/s_"$EX".out"

      	./$EXEC $IN_FILE $USER_FILE &> /dev/null
	
#penalizare daca are fisierul prea mare
	user_lines=$(wc -l < $USER_FILE)
	out_lines=$(wc -l < $OUT_FILE)
	
	if [[ $user_lines -gt $out_lines ]]
	then
		echo "STACK-test_$EX : -5 Fisierul de output prea mare"
		echo
		SCORES=$((SCORES-5))
	fi

#mergem pe numarul de linii al iesirii temei
	while read -r line_user && read -r line_out <&3; 
	do
		if [ "$line_user" == "$line_out" ]
		then
			SCORES=$((SCORES+1))
		fi
	done < $USER_FILE 3<$OUT_FILE

	rm -f $USER_FILE

        if (( $SCORES < 0 ))
        then
                SCORES=0
        fi

        echo "STACK-test_$EX .................... $SCORES/10"
        SCOREST=$((SCOREST + SCORES))

done

for EX in {1..2}
do
	SCOREB=0
	IN_FILE="tests/b_"$EX".in"
	USER_FILE="data.out"
	OUT_FILE="tests/b_"$EX".out"

	./$EXEC $IN_FILE $USER_FILE &> /dev/null
	
#penalizare daca are fisierul prea mare
	user_lines=$(wc -l < $USER_FILE)
	out_lines=$(wc -l < $OUT_FILE)

	
	if [[ $user_lines -gt $out_lines ]]
	then
		echo "BONUS-test_$EX : -5 Fisierul de output prea mare"
		echo
		SCOREB=$(( $SCOREB-5 ))
	fi

#mergem pe numarul de linii al iesirii temei
	while read -r line_user && read -r line_out <&3; 
	do
		if [ "$line_user" == "$line_out" ]
		then
			SCOREB=$(( $SCOREB+1 ))
		fi
	done < $USER_FILE 3<$OUT_FILE

	rm -f $USER_FILE

        if (( $SCOREB < 0 ))
        then
                SCOREB=0
        fi

        echo "BONUS-test_$EX .................... $SCOREB/10"
        SCOREBT=$((SCOREBT + SCOREB))

done

echo "Queue tests: $SCOREQT/40"
echo "Stack tests: $SCOREST/50"
echo "Bonus tests: $SCOREBT/20"
echo "--------------------"
TOTAL=$((SCOREQT + SCOREST + SCOREBT))
echo "Total: $TOTAL/110"

make clean
