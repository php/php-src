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

$inputs = array(12345, 12.3456789000E-10, true, false, "");
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
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
  $iterator++;
}

?>
--EXPECT--
-- Iteration 1 --
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "12345" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "12345" given

-- Iteration 2 --
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "1.23456789E-9" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "1.23456789E-9" given

-- Iteration 3 --
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "1" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "1" given

-- Iteration 4 --
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "" given

-- Iteration 5 --
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "" given
