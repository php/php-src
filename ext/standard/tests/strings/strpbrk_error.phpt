--TEST--
Test strpbrk() function : error conditions
--FILE--
<?php
$haystack = 'This is a Simple text.';

echo "-- Testing strpbrk() function with empty second argument --\n";
try {
    strpbrk($haystack, '');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
-- Testing strpbrk() function with empty second argument --
strpbrk(): Argument #2 ($characters) must be a non-empty string
