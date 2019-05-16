--TEST--
curl_share_close basic test
--SKIPIF--
<?php if( !extension_loaded( 'curl' ) ) print 'skip'; ?>
--FILE--
<?php

$sh = curl_share_init();
//Show that there's a curl_share resource
var_dump($sh);

curl_share_close($sh);
//Show that resource is no longer a curl_share, and is therefore unusable and "closed"
var_dump($sh);

?>
--EXPECTF--
resource(%d) of type (curl_share)
resource(%d) of type (Unknown)
