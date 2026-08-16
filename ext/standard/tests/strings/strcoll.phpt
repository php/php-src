--TEST--
Testing Basic behaviour of strcoll()
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php

 $a = 'a';
 $b = 'A';

setlocale (LC_COLLATE, 'C');
$result = strcoll($a, $b);
if($result > 0) {
    echo "Pass\n";
}
?>
--EXPECTF--
Deprecated: Function strcoll() is deprecated since 8.6, use Collator::compare() instead in %s on line %d
Pass
