--TEST--
Creating instances dynamically
--FILE--
<?php

$arr = array(new StdClass, 'StdClass');

new $arr[0]();
new $arr[1]();

print "ok\n";

?>
--EXPECT--
ok
