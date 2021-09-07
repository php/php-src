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
--EXPECT--
NULL
object(DynamicObject)#2 (0) {
}
