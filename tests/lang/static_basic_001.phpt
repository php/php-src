--TEST--
Static keyword - basic tests
--FILE--
<?php

echo "\nSame variable used as static and non static.\n";
function staticNonStatic() {
	echo "---------\n";
	$a=0;
	echo "$a\n";
	static $a=10;
	echo "$a\n";
	$a++;
}
staticNonStatic();
staticNonStatic();
staticNonStatic();

echo "\nLots of initialisations in the same statement.\n";
function manyInits() {
	static $counter=0;
	echo "------------- Call $counter --------------\n";
	static $a, $b=10, $c=20, $d, $e=30;
	echo "Unitialised      : $a\n";
	echo "Initialised to 10: $b\n";
	echo "Initialised to 20: $c\n";
	echo "Unitialised      : $d\n";
	echo "Initialised to 30: $e\n";
	$a++;
	$b++;
	$c++;
	$d++;
	$e++;
	$counter++;
}
manyInits();
manyInits();
manyInits();

echo "\nUsing static keyword at global scope\n";
for ($i=0; $i<3; $i++) {
   static $s, $k=10;
   echo "$s $k\n";
   $s++;
   $k++;
}
?>
--EXPECT--
Same variable used as static and non static.
---------
0
10
---------
0
11
---------
0
12

Lots of initialisations in the same statement.
------------- Call 0 --------------
Unitialised      : 
Initialised to 10: 10
Initialised to 20: 20
Unitialised      : 
Initialised to 30: 30
------------- Call 1 --------------
Unitialised      : 1
Initialised to 10: 11
Initialised to 20: 21
Unitialised      : 1
Initialised to 30: 31
------------- Call 2 --------------
Unitialised      : 2
Initialised to 10: 12
Initialised to 20: 22
Unitialised      : 2
Initialised to 30: 32

Using static keyword at global scope
 10
1 11
2 12
