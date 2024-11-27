--TEST--
Curl persistent share handle test with invalid option
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init_persistent([CURL_LOCK_DATA_DNS, CURL_LOCK_DATA_CONNECT, 30]);

?>
--EXPECTF--
Fatal error: Uncaught Exception: Could not construct persistent cURL share handle: Unknown share option in %scurl_persistent_share_003.php:3
Stack trace:
#0 %scurl_persistent_share_003.php(3): curl_share_init_persistent(Array)
#1 {main}
  thrown in %scurl_persistent_share_003.php on line 3
