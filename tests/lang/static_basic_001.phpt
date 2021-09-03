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
    echo "Uninitialized    : $a\n";
    echo "Initialized to 10: $b\n";
    echo "Initialized to 20: $c\n";
    echo "Uninitialized    : $d\n";
    echo "Initialized to 30: $e\n";
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
Uninitialized    : 
Initialized to 10: 10
Initialized to 20: 20
Uninitialized    : 
Initialized to 30: 30
------------- Call 1 --------------
Uninitialized    : 1
Initialized to 10: 11
Initialized to 20: 21
Uninitialized    : 1
Initialized to 30: 31
------------- Call 2 --------------
Uninitialized    : 2
Initialized to 10: 12
Initialized to 20: 22
Uninitialized    : 2
Initialized to 30: 32

Using static keyword at global scope
 10
1 11
2 12
