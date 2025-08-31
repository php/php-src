--TEST--
Bug #75717: RecursiveArrayIterator does not traverse arrays by reference
--FILE--
<?php

function flatten(array $nestedArraysAndStrings){
    $flat=[];
    $iter = new RecursiveIteratorIterator(
        new RecursiveArrayIterator($nestedArraysAndStrings));
    foreach($iter as $leaf){ $flat[] = $leaf; }
    return join('', $flat);
}

$noRefs = [[[['some']]],[' nested '],"items"];

$withRefs = []+$noRefs;
$wat = $noRefs[0];
$withRefs[0] = &$wat;

echo flatten($noRefs), "\n";
echo flatten($withRefs), "\n";

?>
--EXPECT--
some nested items
some nested items
