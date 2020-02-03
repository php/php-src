--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\stdclass $x = null) {
    var_dump($x);
};

$x(null);
$x(new \stdclass());

?>
--EXPECTF--
NULL
object(stdClass)#%d (0) {
}
