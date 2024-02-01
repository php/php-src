--TEST--
Test fputcsv() : usage variations - with default enclosure & delimiter of two chars
--FILE--
<?php

/* Testing fputcsv() to write to a file when default enclosure value and delimiter
   of two chars is provided */

echo "*** Testing fputcsv() : with default enclosure & delimiter of two chars ***\n";

$fo = new SplFileObject(__DIR__ . '/SplFileObject_fputcsv_variation13.csv', 'w');

try {
    var_dump($fo->fputcsv(array('water', 'fruit'), ',,', '"'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

unset($fo);

echo "Done\n";
?>
--CLEAN--
<?php
$file = __DIR__ . '/SplFileObject_fputcsv_variation13.csv';
unlink($file);
?>
--EXPECT--
*** Testing fputcsv() : with default enclosure & delimiter of two chars ***
SplFileObject::fputcsv(): Argument #2 ($separator) must be a single character
Done
