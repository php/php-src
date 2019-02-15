--TEST--
Test curl_setopt() CURLOPT_FILE readonly file handle
--CREDITS--
Mark van der Velden
#testfest Utrecht 2009
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
/*
 * Description       : Adds a file which stores the received data from curl_exec();
 * Source code       : ext/curl/multi.c
 * Test documentation: http://wiki.php.net/qa/temp/ext/curl
 */

// Figure out what handler to use
include 'server.inc';
$host = curl_cli_server_start();
if(!empty($host)) {

    // Use the set Environment variable
    $url = "$host/get.inc?test=1";

} else {

    // Create a temporary file for the test
    $tempname = tempnam(sys_get_temp_dir(), 'CURL_HANDLE');
    $url = 'file://'. $tempname;

    // add the test data to the file
    file_put_contents($tempname, "Hello World!\nHello World!");
}


$tempfile	= tempnam(sys_get_temp_dir(), 'CURL_FILE_HANDLE');

$ch = curl_init($url);
$fp = fopen($tempfile, "r"); // Opening 'fubar' with the incorrect readonly flag
curl_setopt($ch, CURLOPT_FILE, $fp);
curl_exec($ch);
curl_close($ch);
is_file($tempfile) and @unlink($tempfile);
isset($tempname) and is_file($tempname) and @unlink($tempname);
?>
--EXPECTF--
Warning: curl_setopt(): the provided file handle is not writable in %s on line %d
Hello World!
Hello World!
