--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\stdclass $x = NULL) {
    var_dump($x);
};

$x(NULL);
$x(new \stdclass);

?>
--EXPECTF--
Deprecated: {closure:%s:%d}(): Implicitly marking parameter $x as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
NULL
object(stdClass)#%d (0) {
}
