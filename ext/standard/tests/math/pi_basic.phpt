--TEST--
Test pi() - basic function test pi()
--INI--
precision=14
--FILE--
<?php
echo pi(), "\n";
echo M_PI, "\n";
// N.B pi() ignores all specified arguments no error
// messages are produced if arguments are spcified.
?>
--EXPECTF--
3.1415926535898
3.1415926535898
