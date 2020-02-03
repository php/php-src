--TEST--
Closure 028: Trying to use lambda directly in foreach
--FILE--
<?php

foreach (function(){ return 1; } as $y) {
    var_dump($y);
}

print "ok\n";

?>
--EXPECT--
ok
