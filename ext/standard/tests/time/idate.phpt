--TEST--
idate() function
--FILE--
<?php
putenv ("TZ=GMT");

$tmp = "UYzymndjHGhgistwLBIW";
for($a = 0;$a < strlen($tmp); $a++){
	echo $tmp{$a}, ': ', idate($tmp{$a}, 1043324459)."\n";
}
?>
--EXPECT--
U: 1043324459
Y: 2003
z: 22
y: 3
m: 1
n: 1
d: 23
j: 23
H: 12
G: 12
h: 12
g: 12
i: 20
s: 59
t: 31
w: 4
L: 0
B: 556
I: 0
W: 4
