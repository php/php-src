--TEST--
General function test
--FILE--
<?php

function a()
{
  echo "hey\n";
}

function b($i)
{
  echo "$i\n";
}


function c($i,$j)
{
  echo "Counting from $i to $j\n";
  for ($k=$i; $k<=$j; $k++) {
    echo "$k\n";
  }
}



a();
b("blah");
a();
b("blah","blah");
c(7,14);

a();


function factorial($n)
{
  if ($n==0 || $n==1) {
    return 1;
  } else {
    return factorial($n-1)*$n;
  }
}


function factorial2($start, $n)
{
  if ($n<=$start) {
    return $start;
  } else {
    return factorial2($start,$n-1)*$n;
  }
}


for ($k=0; $k<10; $k++) {
  for ($i=0; $i<=10; $i++) {
    $n=factorial($i);
    echo "factorial($i) = $n\n";
  }
}


echo "and now, from a function...\n";

function call_fact()
{
  echo "(it should break at 5...)\n";
  for ($i=0; $i<=10; $i++) {
    if ($i == 5) break;
    $n=factorial($i);
    echo "factorial($i) = $n\n";
  }
}

function return4() { return 4; }
function return7() { return 7; }

for ($k=0; $k<10; $k++) {
  call_fact();
}

echo "------\n";
$result = factorial(factorial(3));
echo "$result\n";

$result=factorial2(return4(),return7());
echo "$result\n";

function andi($i, $j)
{
	for ($k=$i ; $k<=$j ; $k++) {
		if ($k >5) continue;
		echo "$k\n";
	}
}

andi (3,10);
--EXPECT--
hey
blah
hey
blah
Counting from 7 to 14
7
8
9
10
11
12
13
14
hey
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
factorial(5) = 120
factorial(6) = 720
factorial(7) = 5040
factorial(8) = 40320
factorial(9) = 362880
factorial(10) = 3628800
and now, from a function...
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
(it should break at 5...)
factorial(0) = 1
factorial(1) = 1
factorial(2) = 2
factorial(3) = 6
factorial(4) = 24
------
720
840
3
4
5
