--TEST--
Bug #73337 (try/catch not working with two exceptions inside a same operation)
--FILE--
<?php
class d { function __destruct() { throw new Exception; } }
try { new d + new d; } catch (Exception $e) { print "Exception properly caught\n"; }
?>
--EXPECT--
Exception properly caught
