--TEST--
Curl persistent share handles cannot be used with curl_share_setopt
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init_persistent([CURL_LOCK_DATA_DNS]);
curl_share_setopt($sh, CURLOPT_SHARE, CURL_LOCK_DATA_CONNECT);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: curl_share_setopt(): Argument #1 ($share_handle) must be of type CurlShareHandle, CurlSharePersistentHandle given in %scurl_persistent_share_005.php:4
Stack trace:
#0 %scurl_persistent_share_005.php(4): curl_share_setopt(Object(CurlSharePersistentHandle), %d, %d)
#1 {main}
  thrown in %scurl_persistent_share_005.php on line 4
