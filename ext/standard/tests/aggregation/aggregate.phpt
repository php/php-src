--TEST--
aggregating everything
--FILE--
<?php

include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate($obj, 'helper');
$obj->do_this();
$obj->do_that();
print $obj->our_prop;

?>
--EXPECT--
I'm alive!
I'm helping!
I'm aggregating!
****
