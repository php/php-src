--TEST--
curl_setopt_array() function - tests setting multiple cURL options with curl_setopt_array()
--CREDITS--
Mattijs Hoitink mattijshoitink@gmail.com
#Testfest Utrecht 2009
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
/*
 * Prototype:     bool curl_setopt_array(resource $ch, array $options)
 * Description:   Sets multiple options for a cURL session.
 * Source:        ext/curl/interface.c
 * Documentation: http://wiki.php.net/qa/temp/ext/curl
 */

// Figure out what handler to use
include 'server.inc';
$host = curl_cli_server_start();
if (!empty($host)) {
    // Use the set Environment variable
    $url = "{$host}/get.php?test=get";
} else {
    // Create a temporary file for the test
    $tempname = tempnam(sys_get_temp_dir(), 'CURL_HANDLE');
    $url = 'file://'. $tempname;
    // add the test data to the file
    file_put_contents($tempname, "Hello World!\nHello World!");
}

// Start the test
echo '== Starting test curl_setopt_array($ch, $options); ==' . "\n";

// curl handler
$ch = curl_init();

// options for the curl handler
$options = array (
    CURLOPT_URL => $url,
    CURLOPT_RETURNTRANSFER => 1
);

ob_start(); // start output buffering

curl_setopt_array($ch, $options);
$returnContent = curl_exec($ch);
curl_close($ch);

var_dump($returnContent);
isset($tempname) and is_file($tempname) and @unlink($tempname);

?>
--EXPECT--
== Starting test curl_setopt_array($ch, $options); ==
string(25) "Hello World!
Hello World!"
