--TEST--
033: Import statement with non-compound name
--FILE--
<?php
use A;
use \B;
?>
--EXPECTF--
