--TEST--
Testing user-defined function in included file
--POST--
--GET--
--FILE--
<?php 
include "016.inc";
MyFunc("Hello");
?>
--EXPECT--
Hello
