--TEST--
union default values (pass)
--FILE--
<?php
function (IFoo or IBar $thing = null) {};

echo "ok";
?>
--EXPECT--
ok
