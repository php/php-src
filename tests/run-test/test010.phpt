--TEST--
STDIN input
--FILE--
<?php
var_dump(stream_get_contents(STDIN));
var_dump(stream_get_contents(fopen('php://stdin', 'r')));
var_dump(file_get_contents('php://stdin'));
?>
--STDIN--
fooBar
use this to input some thing to the php script
--EXPECTF--
string(5%d) "fooBar
use this to input some thing to the php script
"
string(0) ""
string(0) ""
