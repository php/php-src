--TEST--
GH-10810: Fix NUL byte terminating Exception::__toString()
--FILE--
<?php
echo new \Exception("Hello\0World");
?>
--EXPECTF--
Exception: Hello%0World in %s:%d
Stack trace:
#0 {main}
