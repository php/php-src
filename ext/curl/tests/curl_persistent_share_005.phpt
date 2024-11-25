--TEST--
Curl persistent share handles cannot be used with curl_share_setopt
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_persistent_share_init([CURL_LOCK_DATA_DNS]);
curl_share_setopt($sh, CURLOPT_SHARE, CURL_LOCK_DATA_CONNECT);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: curl_share_setopt(): Argument #1 ($share_handle) must be of type CurlShareHandle, CurlPersistentShareHandle given in %scurl_persistent_share_005.php:4
Stack trace:
#0 %scurl_persistent_share_005.php(4): curl_share_setopt(Object(CurlPersistentShareHandle), %d, %d)
#1 {main}
  thrown in %scurl_persistent_share_005.php on line 4
