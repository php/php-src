--TEST--
SPL: RecursiveTreeIterator::setPrefixPart()
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php

$ary = array(
	"a" => array("b"),
	"c" => array("d"),
);

$it = new RecursiveArrayIterator($ary);
$it = new RecursiveTreeIterator($it);
for($i = 0; $i < 6; ++$i) {
	$it->setPrefixPart($i, $i);
}
foreach($it as $k => $v) {
	echo "[$k] => $v\n";
}
try {
	$it->setPrefixPart(-1, "");
	$it->setPrefixPart(6, "");
} catch (OutOfRangeException $e) {
	echo "OutOfRangeException thrown\n";
}
try {
	$it->setPrefixPart(6, "");
} catch (OutOfRangeException $e) {
	echo "OutOfRangeException thrown\n";
}
?>
===DONE===
--EXPECTF--
[a] => 035Array
[0] => 0145b
[c] => 045Array
[0] => 0245d
OutOfRangeException thrown
OutOfRangeException thrown
===DONE===
