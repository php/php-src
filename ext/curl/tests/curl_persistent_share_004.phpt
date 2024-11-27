--TEST--
Curl persistent share handle test with disallowed option
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init_persistent([CURL_LOCK_DATA_COOKIE]);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: curl_share_init_persistent(): Argument #1 ($share_options) CURL_LOCK_DATA_COOKIE is not allowed in %scurl_persistent_share_004.php:3
Stack trace:
#0 %scurl_persistent_share_004.php(3): curl_share_init_persistent(Array)
#1 {main}
  thrown in %scurl_persistent_share_004.php on line 3
