--TEST--
Test curl option CURLOPT_FILE with STDOUT handle
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Writes the value 'test' to a temporary file. Use curl to access this file and store the output in another temporary file. Tests the PHP_CURL_FILE case in curl_write().
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

$log_file = tempnam(sys_get_temp_dir(), 'php-curl-test');

$fp = fopen($log_file, 'w+');
fwrite($fp, "test");
fclose($fp);

$ch = curl_init();
curl_setopt($ch, CURLOPT_FILE, STDOUT);
curl_setopt($ch, CURLOPT_URL, 'file://' . $log_file);
curl_exec($ch);
curl_close($ch);

// cleanup
unlink($log_file);

?>
--EXPECT--
test
