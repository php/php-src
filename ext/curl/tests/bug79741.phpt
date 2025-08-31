--TEST--
Bug #79741: curl_setopt CURLOPT_POSTFIELDS asserts on object with declared properties
--EXTENSIONS--
curl
--FILE--
<?php

class Test {
    public $prop = "value";
}

$ch = curl_init();
curl_setopt($ch, CURLOPT_POSTFIELDS, new Test);

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Error: Object of class Test could not be converted to string in %s:%d
Stack trace:
#0 %s(%d): curl_setopt(Object(CurlHandle), %d, Object(Test))
#1 {main}
  thrown in %s on line %d
