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
NULL
object(stdClass)#%d (0) {
}
