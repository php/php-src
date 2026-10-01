--TEST--
SKIPIF evaluation environment
--SKIPIF--
<?php
if (getenv('TEST_PHP_EVALUATING_SKIPIF') !== '1') {
    echo 'missing SKIPIF environment marker';
}
?>
--FILE--
<?php
var_dump(getenv('TEST_PHP_EVALUATING_SKIPIF'));
?>
--EXPECT--
bool(false)
