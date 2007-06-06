--TEST--
chunk_split() function
--FILE--
<?php
echo chunk_split('abc', 1, '-')."\n";
echo chunk_split('foooooooooooooooo', 5)."\n";
echo chunk_split(str_repeat('X', 2*76))."\n";
echo chunk_split("test", 10, "|end") . "\n";

$a=str_repeat("B", 65535);
$b=1;
$c=str_repeat("B", 65535);
var_dump(chunk_split($a,$b,$c));

$a=str_repeat("B", 65537);
$b=1;
$c=str_repeat("B", 65537);
var_dump(chunk_split($a,$b,$c));


?>
--EXPECT--
a-b-c-
foooo
ooooo
ooooo
oo

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

test|end
bool(false)
bool(false)
