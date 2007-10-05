--TEST--
Test split()
--FILE--
<?php
	$a=split("[[:space:]]","this is	a
test");
	echo count($a) . "\n";
	for ($i = 0; $i < count($a); $i++) {
          echo $a[$i] . "\n";
        }
?>
--EXPECT--
4
this
is
a
test
