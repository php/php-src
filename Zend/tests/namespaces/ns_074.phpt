--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (?\stdClass $x = NULL) {
    var_dump($x);
};

class stdclass extends \stdClass { }

$x(NULL);
$x(new stdclass);
$x(new \stdClass);

?>
--EXPECTF--
NULL
object(foo\stdclass)#%d (0) {
}
object(stdClass)#%d (0) {
}
