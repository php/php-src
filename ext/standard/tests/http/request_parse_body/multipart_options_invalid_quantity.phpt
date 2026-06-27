--TEST--
request_parse_body() invalid quantity
--FILE--
<?php

foreach ([
    ['upload_max_filesize', '1GB'],
    ['upload_max_filesize', ''],
    ['post_max_size', '1GB'],
    ['post_max_size', ''],
] as [$option, $value]) {
    try {
        request_parse_body(options: [
            $option => $value,
        ]);
    } catch (Throwable $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
ValueError: Invalid "upload_max_filesize" value in $options argument: Invalid quantity "1GB": unknown multiplier "B"
ValueError: Invalid "upload_max_filesize" value in $options argument: Invalid quantity "": no valid leading digits
ValueError: Invalid "post_max_size" value in $options argument: Invalid quantity "1GB": unknown multiplier "B"
ValueError: Invalid "post_max_size" value in $options argument: Invalid quantity "": no valid leading digits
