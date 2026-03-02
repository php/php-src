--TEST--
GH-16359 - curl_setopt with CURLOPT_WRITEFUNCTION and no user fn
--EXTENSIONS--
curl
--FILE--
<?php
$log_file = tempnam(sys_get_temp_dir(), 'php-curl-CURLOPT_WRITEFUNCTION-trampoline');
$fp = fopen($log_file, 'w+');
fwrite($fp, "test");
$ch = curl_init();
curl_setopt($ch, CURLOPT_WRITEFUNCTION, null);
curl_setopt($ch, CURLOPT_URL, 'file://' . $log_file);
curl_exec($ch);
?>
--EXPECT--
test
