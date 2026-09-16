--TEST--
Creating instances dynamically
--FILE--
<?php

$arr = array(new stdClass, 'stdClass');

new $arr[0]();
new $arr[1]();

print "ok\n";

?>
--EXPECT--
ok
