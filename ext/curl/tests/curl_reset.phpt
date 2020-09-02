--TEST--
Test curl_reset
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip";
if (!function_exists("curl_reset")) exit("skip curl_reset doesn't exists (require libcurl >= 7.12.1)");
?>
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

curl_reset($ch);
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
testtest
