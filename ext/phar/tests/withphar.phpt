--TEST--
Phar: phar run with ext/phar with default stub
--EXTENSIONS--
phar
--FILE--
<?php
include __DIR__ . '/files/nophar.phar';
?>
--EXPECT--
in b
<?php include "b/c.php";
in d
