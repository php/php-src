--TEST--
curl_share_close basic test
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init();
//Show that there's a curl_share object
var_dump($sh);

curl_share_close($sh);
var_dump($sh);

?>
--EXPECT--
object(CurlShareHandle)#1 (0) {
}
object(CurlShareHandle)#1 (0) {
}
