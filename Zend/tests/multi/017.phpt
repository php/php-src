--TEST--
intersection default values (pass)
--FILE--
<?php
function (IFoo and IBar $thing = null) {};

echo "ok";
?>
--EXPECT--
ok
