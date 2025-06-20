--TEST--
Phar: phar run with ext/phar with default stub
--EXTENSIONS--
phar
--FILE--
<?php
include __DIR__ . '/files/nophar.phar';
?>
--EXPECTF--
Deprecated: class_exists(): Passing int to parameter #2 ($autoload) of type bool is deprecated in %snophar.phar on line 5
in b
<?php include "b/c.php";
in d
