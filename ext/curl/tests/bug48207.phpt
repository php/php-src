--TEST--
Test curl_setopt() CURLOPT_FILE readonly file handle
--CREDITS--
Mark van der Velden
#testfest Utrecht 2009
--EXTENSIONS--
curl
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
$fp = fopen($tempfile, "r"); // Opening 'fubar' with the incorrect readonly flag

$ch = curl_init($url);

foreach ([
    CURLOPT_FILE,
    CURLOPT_WRITEHEADER,
    CURLOPT_STDERR,
] as $option) {
    try {
        curl_setopt($ch, $option, $fp);
    } catch (ValueError $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
}

curl_exec($ch);
is_file($tempfile) and @unlink($tempfile);
isset($tempname) and is_file($tempname) and @unlink($tempname);
?>
--EXPECT--
ValueError: curl_setopt(): The file handle provided for CURLOPT_FILE must be writable
ValueError: curl_setopt(): The file handle provided for CURLOPT_WRITEHEADER must be writable
ValueError: curl_setopt(): The file handle provided for CURLOPT_STDERR must be writable
Hello World!
Hello World!
