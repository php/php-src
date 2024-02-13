--TEST--
Test CurlMultiHandle
--CREDITS--
Based on curl_multi_init_basic.phpt by:
Mark van der Velden
#testfest Utrecht 2009
--EXTENSIONS--
curl
--FILE--
<?php

// start testing
echo "*** Testing CurlMultiHandle ***\n";

//create the multiple cURL handle
$mh = new CurlMultiHandle();
var_dump($mh);
--EXPECT--
*** Testing CurlMultiHandle ***
object(CurlMultiHandle)#1 (0) {
}
