--TEST--
Generators using symtables must not leak
--FILE--
<?php

function gen() {
	$bar = ["some complex var"];
	${"f"."oo"} = "force symtable usage";
	var_dump($bar);
	yield;
}

gen()->valid();

?>
--EXPECT--
array(1) {
  [0]=>
  string(16) "some complex var"
}
