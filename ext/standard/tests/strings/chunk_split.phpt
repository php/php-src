--TEST--
chunk_split() function
--POST--
--GET--
--FILE--
<?php
echo chunk_split('abc', 1, '-')."\n";
echo chunk_split('foooooooooooooooo', 5)."\n";
echo chunk_split(str_repeat('X', 2*76))."\n";
?>
--EXPECT--
a-b-c-
foooo
ooooo
ooooo
oo

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
