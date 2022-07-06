--TEST--
Bug #12776 (array_walk crash)
--FILE--
<?php

function test($val,$key)
{
    global $globalArray;
    $globalArray[]=$key; // this will end up crashing
    $globalArray[]=(string)$key; // this will end up OK
    print "val: $val; key: $key\n"; flush();
}

$arr=array('k'=>'v');
array_walk($arr,'test');

print "First value: ".$globalArray[0];

print "\nDone\n";

?>
--EXPECT--
val: v; key: k
First value: k
Done
