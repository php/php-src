--TEST--
Testing user-defined function in included file
--FILE--
<?php
include "016.inc";
MyFunc("Hello");
?>
--EXPECT--
Hello
