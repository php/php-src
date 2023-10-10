--TEST--
Bug #52820 (writes to fopencookie FILE* not committed when seeking the stream)
--EXTENSIONS--
curl
zend_test
--SKIPIF--
<?php
/* unfortunately no standard function does a cast to FILE*, so we need
 * curl to test this */
$handle=curl_init('file:///i_dont_exist/');
curl_setopt($handle, CURLOPT_VERBOSE, true);
curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
if (!@curl_setopt($handle, CURLOPT_STDERR, fopen("php://memory", "w+")))
    die("skip fopencookie not supported on this platform");
if (getenv('CIRCLECI')) die('xfail Broken on CircleCI');
?>
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
?>
--EXPECTREGEX--
temp stream \(close after\):
About to rewind!
(\* processing: file:\/\/\/i_dont_exist\/\n)?\* Couldn't open file \/i_dont_exist\/
\* Closing connection( -?\d+)?

memory stream \(close after\):
About to rewind!
(\* processing: file:\/\/\/i_dont_exist\/\n)?\* Couldn't open file \/i_dont_exist\/
\* Closing connection( -?\d+)?

temp stream \(leak\):
About to rewind!
(\* processing: file:\/\/\/i_dont_exist\/\n)?\* Couldn't open file \/i_dont_exist\/
\* Closing connection( -?\d+)?

memory stream \(leak\):
About to rewind!
(\* processing: file:\/\/\/i_dont_exist\/\n)?\* Couldn't open file \/i_dont_exist\/
\* Closing connection( -?\d+)?

Done\.
