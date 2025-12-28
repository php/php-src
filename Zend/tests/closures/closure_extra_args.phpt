--TEST--
Immediately invoked closure with extra args
--FILE--
<?php

(function() {})(new stdClass);

?>
===DONE===
--EXPECT--
===DONE===
