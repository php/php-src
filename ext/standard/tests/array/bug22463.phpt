--TEST--
Bug #22463 (array_reduce() segfaults)
--FILE--
<?php

function a($ary) {
	return (is_array($ary) ? array_reduce($ary, 'cb', 0) : 1);
}

function cb($v, $elem) {
	return $v + a($elem); 
}

$ary = array(
	array(
		array(
			array(
				array(
					array(0, 1, 2, 3, 4)
				)
			)
		)
	)
);

var_dump(a($ary));
?>
--EXPECT--
int(5)
