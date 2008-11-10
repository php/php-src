--TEST--
Testing type-hinted lambda parameter inside namespace
--FILE--
<?php

namespace foo;

$x = function (\stdclass $x = NULL) { 
	var_dump($x);	
};

class stdclass { }

$x(NULL);
$x(new stdclass);
$x(new stdclass);

?>
--EXPECTF--
NULL
object(foo\stdClass)#%d (0) {
}
object(foo\stdClass)#%d (0) {
}
