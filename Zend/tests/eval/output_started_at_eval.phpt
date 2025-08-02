--TEST--
Output start at eval()
--FILE--
<?php
eval('echo "Foo\n";');
header('Foo: Bar');
?>
--EXPECTF--
Foo

Warning: Cannot modify header information - headers already sent by (output started at %s(2) : eval()'d code:1) in %s on line %d
