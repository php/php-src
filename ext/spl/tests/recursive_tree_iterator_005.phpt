--TEST--
SPL: RecursiveTreeIterator and binary vs unicode (PHP 6.0+) 
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php

$ary = array(
	0 => array(
		(binary) "binary",
		"abc2",
		1,
	),
	(binary) "binary" => array(
		2,
		"b",
		3 => array(
			4,
			"c",
		),
		"4abc" => array(
			4,
			"c",
		),
	),
);

$it = new RecursiveTreeIterator(new RecursiveArrayIterator($ary), 0);
foreach($it as $k => $v) {
	var_dump($v);
}
echo "\n----------------\n\n";
foreach($it as $k => $v) {
	var_dump($k);
}
echo "\n----------------\n\n";
echo "key, getEntry, current:\n";
foreach($it as $k => $v) {
	var_dump($it->key(), $it->getEntry(), $it->current());
}
?>
===DONE===
--EXPECT--
string(7) "|-Array"
string(10) "| |-binary"
unicode(8) "| |-abc2"
unicode(5) "| \-1"
string(7) "\-Array"
unicode(5) "  |-2"
unicode(5) "  |-b"
string(9) "  |-Array"
unicode(7) "  | |-4"
unicode(7) "  | \-c"
string(9) "  \-Array"
unicode(7) "    |-4"
unicode(7) "    \-c"

----------------

unicode(3) "|-0"
unicode(5) "| |-0"
unicode(5) "| |-1"
unicode(5) "| \-2"
string(8) "\-binary"
unicode(5) "  |-0"
unicode(5) "  |-1"
unicode(5) "  |-3"
unicode(7) "  | |-0"
unicode(7) "  | \-1"
unicode(8) "  \-4abc"
unicode(7) "    |-0"
unicode(7) "    \-1"

----------------

key, getEntry, current:
unicode(3) "|-0"
string(5) "Array"
string(7) "|-Array"
unicode(5) "| |-0"
string(6) "binary"
string(10) "| |-binary"
unicode(5) "| |-1"
unicode(4) "abc2"
unicode(8) "| |-abc2"
unicode(5) "| \-2"
unicode(1) "1"
unicode(5) "| \-1"
string(8) "\-binary"
string(5) "Array"
string(7) "\-Array"
unicode(5) "  |-0"
unicode(1) "2"
unicode(5) "  |-2"
unicode(5) "  |-1"
unicode(1) "b"
unicode(5) "  |-b"
unicode(5) "  |-3"
string(5) "Array"
string(9) "  |-Array"
unicode(7) "  | |-0"
unicode(1) "4"
unicode(7) "  | |-4"
unicode(7) "  | \-1"
unicode(1) "c"
unicode(7) "  | \-c"
unicode(8) "  \-4abc"
string(5) "Array"
string(9) "  \-Array"
unicode(7) "    |-0"
unicode(1) "4"
unicode(7) "    |-4"
unicode(7) "    \-1"
unicode(1) "c"
unicode(7) "    \-c"
===DONE===
