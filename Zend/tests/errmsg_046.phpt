--TEST--
errmsg: cannot use 'static' as parent class name
--FILE--
<?php

class test extends static {
}

echo "Done\n";
?>
--EXPECTF-- 
Fatal error: Cannot use 'static' as class name as it is reserved in %s on line %d
