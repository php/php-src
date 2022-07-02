--TEST--
Test curl option CURLOPT_WRITEFUNCTION
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Writes the value 'test' to a temporary file. Use curl to access this file, passing the output into a callback. Tests the PHP_CURL_USER case in curl_write.
--EXTENSIONS--
curl
--FILE--
<?php

function curl_callback($curl_handle, $received_data)
{
    echo $received_data;
    return strlen($received_data);
}

$log_file = tempnam(sys_get_temp_dir(), 'php-curl-test');

$fp = fopen($log_file, 'w+');
fwrite($fp, "test");
fclose($fp);

$ch = curl_init();
curl_setopt($ch, CURLOPT_WRITEFUNCTION, 'curl_callback');
curl_setopt($ch, CURLOPT_URL, 'file://' . $log_file);
curl_exec($ch);
curl_close($ch);

// cleanup
unlink($log_file);

?>
--EXPECT--
test
