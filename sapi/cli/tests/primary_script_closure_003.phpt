--TEST--
Execute closures returned by the primary script (From different file)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

return include "primary_script_closure.inc";

?>
--EXPECT--
Called
