--TEST--
Function Argument Parsing #003
--FILE--
<?php
const a = 10;

function t1($a = 1 + 1, $b = 1 << 2, $c = "foo" . "bar", $d = a * 10) {
	var_dump($a, $b, $c, $d);
}

t1();
?>
--EXPECT--
int(2)
int(4)
string(6) "foobar"
int(100)
