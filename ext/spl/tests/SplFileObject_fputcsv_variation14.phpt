--TEST--
Test fputcsv() : usage variations - with enclosure & delimiter of two chars
--FILE--
<?php

/* Testing fputcsv() to write to a file when default enclosure value and delimiter
   of two chars is provided and file is opened in read only mode */

echo "*** Testing fputcsv() : with enclosure & delimiter of two chars and file opened in read mode ***\n";

$fo = new SplFileObject(__DIR__ . '/SplFileObject_fputcsv.csv', 'w');

var_dump($fo->fputcsv(array('water', 'fruit'), ',,', '""'));

unset($fo);

echo "Done\n";
?>
--CLEAN--
<?php
$file = __DIR__ . '/SplFileObject_fputcsv.csv';
unlink($file);
?>
--EXPECTF--
*** Testing fputcsv() : with enclosure & delimiter of two chars and file opened in read mode ***

Warning: SplFileObject::fputcsv(): enclosure must be a character in %s on line %d
bool(false)
Done
