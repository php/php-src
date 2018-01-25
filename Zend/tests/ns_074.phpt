--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\stdclass $x = NULL) {
	var_dump($x);
};

class stdclass extends \stdclass { }

$x(NULL);
$x(new stdclass);
$x(new \stdclass);

?>
--EXPECTF--
NULL
object(foo\stdclass)#%d (0) {
}
object(stdClass)#%d (0) {
}
