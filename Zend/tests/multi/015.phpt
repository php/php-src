--TEST--
union default values (pass)
--FILE--
<?php
function (IFoo | IBar $thing = null) {};

echo "ok";
?>
--EXPECT--
ok
