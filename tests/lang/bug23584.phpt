--TEST--
Bug #23584 (error line numbers off by one when using #!php)
--FILE--
#!php
<?php

error_reporting(E_ALL);

echo $foo;

?>
--EXPECTF--
Warning: Undefined variable $foo (This will become an error in PHP 9.0) in %s on line 6
