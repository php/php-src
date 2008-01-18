--TEST--
Phar: phar run without pecl/phar with default stub
--SKIPIF--
<?php if (extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/nophar.phar';
?>
===DONE===
--EXPECT--
in b
<?php include "b/c.php";
in d
===DONE===
