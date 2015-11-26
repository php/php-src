--TEST--
Bug #23584 (error line numbers off by one when using #!php)
--FILE--
#!php
<?php

error_reporting(E_ALL);

echo $foo;

?>
--EXPECTREGEX--
Notice: Undefined variable:.*foo in .* on line 6
