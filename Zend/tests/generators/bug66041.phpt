--TEST--
Bug #66041: list() fails to unpack yielded ArrayAccess object
--FILE--
<?php
function dumpElement() {
    list($value) = yield;
    var_dump($value);
};

$fixedArray = new SplFixedArray(1);
$fixedArray[0] = 'the element';

$generator = dumpElement();
$generator->send($fixedArray);
?>
--EXPECT--
string(11) "the element"
