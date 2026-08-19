--TEST--
Test strpbrk() function : error conditions
--FILE--
<?php
$haystack = 'This is a Simple text.';

echo "-- Testing strpbrk() function with empty second argument --\n";
try {
    strpbrk($haystack, '');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
-- Testing strpbrk() function with empty second argument --
ValueError: strpbrk(): Argument #2 ($characters) must be a non-empty string
