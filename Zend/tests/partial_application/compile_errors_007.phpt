--TEST--
Partial application compile errors: mix application with unpack (place holder before)
--FILE--
<?php
foo(..., ...["foo" => "bar"]);
?>
--EXPECTF--
Fatal error: Cannot combine partial application and unpacking %s on line %d

