--TEST--
SPL: RecursiveTreeIterator(void)
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php
try {
    new RecursiveTreeIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError: RecursiveTreeIterator::__construct() expects at least 1 argument, 0 given
