--TEST--
Test curl option CURLOPT_FILE 
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Writes the value 'test' to a temporary file. Use curl to access this file and store the output in another temporary file. Tests the PHP_CURL_FILE case in curl_write().
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

$test_file = tempnam(sys_get_temp_dir(), 'php-curl-test');
$log_file = tempnam(sys_get_temp_dir(), 'php-curl-test');

$fp = fopen($log_file, 'w+');
fwrite($fp, "test");
fclose($fp);

$testfile_fp = fopen($test_file, 'w+');

$ch = curl_init();
curl_setopt($ch, CURLOPT_FILE, $testfile_fp);
curl_setopt($ch, CURLOPT_URL, 'file://' . $log_file);
curl_exec($ch);
curl_close($ch);

fclose($testfile_fp);

echo file_get_contents($test_file);

// cleanup
unlink($test_file);
unlink($log_file);

?>
--EXPECT--
test
