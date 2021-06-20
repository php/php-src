--TEST--
Bug #71930 (_zval_dtor_func: Assertion `(arr)->gc.refcount <= 1' failed)
--EXTENSIONS--
curl
--FILE--
<?php

class A {
    public static function dummy() {
    }
}

$a = array();
$a[] = "A";
$a[] = "dummy";

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_HEADERFUNCTION, $a);

set_error_handler($a);
set_error_handler(function()use($ch1){});
set_error_handler(function(){});
?>
okey
--EXPECT--
okey
