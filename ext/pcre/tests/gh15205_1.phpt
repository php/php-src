--TEST--
GH-15205: UAF when destroying RegexIterator after pcre request shutdown
--CREDITS--
YuanchengJiang
--FILE--
<?php
$array = new ArrayIterator(array('a', array('b', 'c')));
$regex = [new RegexIterator($array, '/Array/')];
echo "Done\n";
?>
--EXPECT--
Done
