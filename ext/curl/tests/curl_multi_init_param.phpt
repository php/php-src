--TEST--
Test curl_multi_init() fail if any parameter is passed
--CREDITS--
Paulo Eduardo <pauloelr [at] gmail [dot] com>
#testfest SP 2014
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php
/* Prototype         : resource curl_multi_init(void)
 * Description       : Returns a new cURL multi handle
 * Source code       : ext/curl/multi.c
 * Test documentation:  http://wiki.php.net/qa/temp/ext/curl
 */

// start testing

//create the multiple cURL handle
$mh = curl_multi_init('test');
var_dump($mh);

?>
===DONE===
--EXPECTF--
Warning: curl_multi_init() expects exactly 0 parameters, %d given in %s on line %d
NULL
===DONE===
