--TEST--
named argument named 'from' remains usable
--FILE--
<?php
function f($from) { echo $from; }
f(from: 42);
?>
--EXPECT--
42
