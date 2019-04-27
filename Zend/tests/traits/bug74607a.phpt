--TEST--
Bug #74607 (Traits enforce different inheritance rules - number of required parameters)
--FILE--
<?php

abstract class L1{
abstract function m3($x);
}

trait L2t{
function m3($x, $y = 0){}
}

class L2 extends L1{
use L2t;
}

echo "DONE";

?>
--EXPECT--
DONE
