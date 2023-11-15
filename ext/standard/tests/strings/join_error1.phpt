--TEST--
join() function - error when passing null as the array parameter
--FILE--
<?php
try {
    join('', null);
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
join(): Argument #2 ($array) must be of type array, null given
