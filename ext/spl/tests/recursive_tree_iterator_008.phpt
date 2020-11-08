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
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $it->setPrefixPart(6, "");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
[a] => 035Array
[0] => 0145b
[c] => 045Array
[0] => 0245d
RecursiveTreeIterator::setPrefixPart(): Argument #1 ($part) must be a RecursiveTreeIterator::PREFIX_* constant
RecursiveTreeIterator::setPrefixPart(): Argument #1 ($part) must be a RecursiveTreeIterator::PREFIX_* constant
