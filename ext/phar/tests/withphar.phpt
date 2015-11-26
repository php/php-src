--TEST--
Phar: phar run with pecl/phar with default stub
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/files/nophar.phar';
?>
===DONE===
--EXPECT--
in b
<?php include "b/c.php";
in d
===DONE===
