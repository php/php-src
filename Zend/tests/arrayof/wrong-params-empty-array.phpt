--TEST--
test arrayof empty array
--FILE--
<?php
function test(stdClass[] $array = []) {
	return $array;
}

var_dump(test());
?>
--EXPECT--
array(0) {
}



