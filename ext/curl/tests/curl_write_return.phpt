--TEST--
Test curl option CURLOPT_RETURNTRANSFER
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Writes the value 'test' to a temporary file. Use curl to access this file and have it return the content from curl_exec(). Tests the PHP_CURL_RETURN case
of curl_write().
--EXTENSIONS--
curl
--FILE--
<?php

$log_file = tempnam(sys_get_temp_dir(), 'php-curl-test');

$fp = fopen($log_file, 'w+');
fwrite($fp, "test");
fclose($fp);

$ch = curl_init();
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_URL, 'file://' . $log_file);
$result = curl_exec($ch);
curl_close($ch);

echo $result;

// cleanup
unlink($log_file);

?>
--EXPECT--
test
