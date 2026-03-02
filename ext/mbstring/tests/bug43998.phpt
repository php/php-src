--TEST--
Test mb_strtolower() function : Two error messages returned for incorrect encoding for mb_strto[upper|lower]
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Two error messages returned for incorrect encoding for mb_strto[upper|lower]
 * Bug now appears to be fixed
 */

$sourcestring = 'Hello, World';

$input = "";
try {
    var_dump( mb_strtolower($sourcestring, $input) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_strtoupper($sourcestring, $input) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "" given
