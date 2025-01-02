--TEST--
Curl persistent share handles must be initialized with a non-empty $share_opts
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init_persistent([]);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: curl_share_init_persistent(): Argument #1 ($share_options) must not be empty in %scurl_persistent_share_006.php:3
Stack trace:
#0 %scurl_persistent_share_006.php(3): curl_share_init_persistent(Array)
#1 {main}
  thrown in %scurl_persistent_share_006.php on line 3
