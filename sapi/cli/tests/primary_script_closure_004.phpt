--TEST--
Execute closures returned by the primary script (Included file returns closure, but primary script does not)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$closure = include "primary_script_closure.inc";

?>
--EXPECT--
