--TEST--
Bug #79741: curl_setopt CURLOPT_POSTFIELDS asserts on object with declared properties
--FILE--
<?php

class Test {
	public $prop = "value";
}

$ch = curl_init();
curl_setopt($ch, CURLOPT_POSTFIELDS, new Test);

?>
===DONE===
--EXPECT--
===DONE===
