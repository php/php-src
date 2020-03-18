--TEST--
Bug #52820 (writes to fopencookie FILE* not committed when seeking the stream)
--SKIPIF--
<?php
if (!function_exists('zend_leak_variable'))
   die("skip only for debug builds");
/* unfortunately no standard function does a cast to FILE*, so we need
 * curl to test this */
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
$handle=curl_init('file:///i_dont_exist/');
curl_setopt($handle, CURLOPT_VERBOSE, true);
curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
if (!curl_setopt($handle, CURLOPT_STDERR, fopen("php://memory", "w+")))
    die("skip fopencookie not supported on this platform");
--FILE--
<?php
function do_stuff($url) {
    $handle=curl_init('file:///i_dont_exist/');
    curl_setopt($handle, CURLOPT_VERBOSE, true);
    curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($handle, CURLOPT_STDERR, $o = fopen($url, "w+"));
    curl_exec($handle);
    echo "About to rewind!\n";
    rewind($o);
    echo stream_get_contents($o);
    return $o;
}

echo "temp stream (close after):\n";
fclose(do_stuff("php://temp"));

echo "\nmemory stream (close after):\n";
fclose(do_stuff("php://memory"));

echo "\ntemp stream (leak):\n";
zend_leak_variable(do_stuff("php://temp"));

echo "\nmemory stream (leak):\n";
zend_leak_variable(do_stuff("php://memory"));

echo "\nDone.\n";
--EXPECTF--
temp stream (close after):
About to rewind!
* Couldn't open file /i_dont_exist/
* Closing connection%A%d

memory stream (close after):
About to rewind!
* Couldn't open file /i_dont_exist/
* Closing connection%A%d

temp stream (leak):
About to rewind!
* Couldn't open file /i_dont_exist/
* Closing connection%A%d

memory stream (leak):
About to rewind!
* Couldn't open file /i_dont_exist/
* Closing connection%A%d

Done.
