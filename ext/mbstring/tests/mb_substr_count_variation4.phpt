--TEST--
Test mb_substr_count() function : variation - pass a $needle that overlaps in $haystack
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Pass mb_substr_count() a $needle that overlaps in $haystack and see whether
 * it counts only the first occurrence or all other occurrences regardless whether they
 * were part of previous match
 */

echo "*** Testing mb_substr_count() : usage variations ***\n";


echo "\n-- ASCII String --\n";
$string_ascii = 'abcabcabc';
var_dump(mb_substr_count($string_ascii, 'abcabc')); //needle overlaps in haystack

echo "\n-- Multibyte String --\n";
$string_mb = base64_decode('5pel5pys6Kqe5pel5pys6Kqe5pel5pys6Kqe');
$needle_mb = base64_decode('5pel5pys6Kqe5pel5pys6Kqe');
var_dump(mb_substr_count($string_mb, $needle_mb, 'utf-8'));


echo "Done";
?>
--EXPECT--
*** Testing mb_substr_count() : usage variations ***

-- ASCII String --
int(1)

-- Multibyte String --
int(1)
Done
