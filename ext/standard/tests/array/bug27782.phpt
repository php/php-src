--TEST--
Bug #27782 (each(), next(), prev() mange array position)
--FILE--
<?php
$a = array("a", "b", "c");
reset($a);

while (next($a) !== false);

echo current($a) . "\n";
echo prev($a) . "\n";

reset($a);

while (list(,$foo) = each($a)) {
	echo $foo . "\n";
}
echo current($a) . "\n";

while ($foo = prev($a)) {
	echo $foo . "\n";
}
?>
--EXPECT--
c
b
a
b
c
c
b
a
