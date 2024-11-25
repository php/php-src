--TEST--
Curl persistent share handle test with disallowed option
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_persistent_share_init([CURL_LOCK_DATA_COOKIE]);

?>
--EXPECTF--
Fatal error: Uncaught Exception: CURL_LOCK_DATA_COOKIE is not allowed with persistent curl share handles in %scurl_persistent_share_004.php:3
Stack trace:
#0 %scurl_persistent_share_004.php(3): curl_persistent_share_init(Array)
#1 {main}
  thrown in %scurl_persistent_share_004.php on line 3
