--TEST--
GHSA-2cv3-554h-jpmh post_max_size bypass - multipart
--EXTENSIONS--
zend_test
--INI--
post_max_size=100K
--FILE--
<?php

function make_post_data($value) {
    $data = ""
        . "--b\r\n"
        . "Content-Disposition: form-data; name=\"a\"\r\n"
        . "Content-Type: application/octet-stream\r\n"
        . "\r\n"
        . "$value\r\n"
        . "\r\n"
        . "--b--\r\n"
        ;
    $stream = fopen('data:text/plain;base64,' . base64_encode($data), 'r');
    return $stream;
}

const FILLUNIT = 1024 * 5;

zend_test_handle_post_data(make_post_data('hello'), content_type: 'multipart/form-data; boundary=b', content_length: 0);
var_dump(strlen($_POST['a']));

zend_test_handle_post_data(make_post_data(str_repeat('a', 1*1024*1024)), content_type: 'multipart/form-data; boundary=b', content_length: 0);
var_dump(strlen($_POST['a'] ?? '') < 100*1024);

?>
--EXPECTF--
int(7)

Warning: Actual POST length does not match Content-Length, and exceeds 102400 bytes in %s on line %d
bool(true)
