--TEST--
SPL: RecursiveTreeIterator methods
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php

$ary = array(
	0 => array(
		"a",
		1,
	),
	"a" => array(
		2,
		"b",
		3 => array(
			4,
			"c",
		),
		"3" => array(
			4,
			"c",
		),
	),
);

$it = new RecursiveTreeIterator(new RecursiveArrayIterator($ary));
foreach($it as $k => $v) {
	echo '[' . $it->key() . '] => ' . $it->getPrefix() . $it->getEntry() . $it->getPostfix() . "\n";
}
?>
===DONE===
--EXPECT--
[0] => |-Array
[0] => | |-a
[1] => | \-1
[a] => \-Array
[0] =>   |-2
[1] =>   |-b
[3] =>   \-Array
[0] =>     |-4
[1] =>     \-c
===DONE===
