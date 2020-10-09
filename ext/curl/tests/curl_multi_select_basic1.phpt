--TEST--
Test curl_multi_select()
--CREDITS--
Ivo Jansch <ivo@ibuildings.com>
#testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

//create the multiple cURL handle
$mh = curl_multi_init();
echo curl_multi_select($mh)."\n";

curl_multi_close($mh);
?>
--EXPECTF--
%r(0|-1)%r
