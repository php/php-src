--TEST--
PFA compile errors: can not use unpacking in PFA, including with variadic placeholdres
--FILE--
<?php
foo(...["foo" => "bar"], ...);
?>
--EXPECTF--
Fatal error: Cannot combine partial application and unpacking in %s on line %d
