--TEST--
mb_split() compatibility test 1 (counterpart: ext/standard/tests/reg/009.phpt)
--POST--
--GET--
--FILE--
<?php
	$a=mb_split("[[:space:]]","this is	a
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
