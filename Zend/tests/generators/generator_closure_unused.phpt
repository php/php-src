--TEST--
Closures can be unused generators
--FILE--
<?php
(function(){yield;})();
echo "ok\n";
?>
--EXPECT--
ok
