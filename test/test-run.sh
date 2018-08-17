#!/bin/bash

PARTICLE=../particle
TEST_SRC=test.par
RESULT=result.txt
ANSWER=answer.txt

# Run test
$PARTICLE $TEST_SRC > $RESULT

# Check result
answers=(`cat $ANSWER`)
results=(`cat $RESULT`)

total=${#results[@]}
ok_count=0
ng_count=0
for ((i = 0; i < ${#results[@]}; i++)) {
    ret=${results[i]}
	ans=${answers[i]}

	if [ "$ret" = "$ans" ]; then
		ok_count=`expr $ok_count + 1`
	else
		echo "NG (No.$i) expected = $ans, ret = $ret"
		ng_count=`expr $ng_count + 1`
	fi
}

echo "--------------------------------------"
echo "Total:$total OK:$ok_count NG:$ng_count"

rm $RESULT