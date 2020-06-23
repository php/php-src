--TEST--
Test gauss_sum() - negative parameter test
--FILE--
<?php
try {
    gauss_sum(-1);
} catch(ValueError $exception) {
    echo $exception->getMessage()."\n";
}
?>
--EXPECT--
gauss_sum(): Argument #1 ($number) must be greater than 0
