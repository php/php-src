--TEST--
request_parse_body() invalid quantity
--FILE--
<?php

foreach (['1GB', ''] as $value) {
    try {
        request_parse_body(options: [
            'upload_max_filesize' => $value,
        ]);
    } catch (Throwable $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
ValueError: Invalid "upload_max_filesize" value in $options argument: Invalid quantity "1GB": unknown multiplier "B"
ValueError: Invalid "upload_max_filesize" value in $options argument: Invalid quantity "": no valid leading digits
