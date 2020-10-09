--TEST--
SPL FixedArray offsetExists behaviour on a negative index
--CREDITS--
PHPNW TestFest 2009 - Ben Longden
--FILE--
<?php
$array = new SplFixedArray(5);
if($array->offsetExists(-10) === false) {
    echo 'PASS';
}
?>
--EXPECT--
PASS
