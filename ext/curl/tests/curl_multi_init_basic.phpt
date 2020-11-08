--TEST--
Test curl_multi_init()
--CREDITS--
Mark van der Velden
#testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

// start testing
echo "*** Testing curl_multi_init(void); ***\n";

//create the multiple cURL handle
$mh = curl_multi_init();
var_dump($mh);

curl_multi_close($mh);
var_dump($mh);
?>
--EXPECT--
*** Testing curl_multi_init(void); ***
object(CurlMultiHandle)#1 (0) {
}
object(CurlMultiHandle)#1 (0) {
}
