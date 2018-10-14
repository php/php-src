--TEST--
Test curl_multi_select()
--CREDITS--
Ivo Jansch <ivo@ibuildings.com>
#testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php
/* Prototype         : resource curl_multi_select($mh, $timeout=1.0])
 * Description       : Get all the sockets associated with the cURL extension, which can then be
 *                     "selected"
 * Source code       : ?
 * Test documentation: http://wiki.php.net/qa/temp/ext/curl
 */


//create the multiple cURL handle
$mh = curl_multi_init();
echo curl_multi_select($mh)."\n";

curl_multi_close($mh);
?>
===DONE===
--EXPECTF--
%r(0|-1)%r
===DONE===
