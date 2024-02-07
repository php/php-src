--TEST--
request_parse_body() invalid quantity
--FILE--
<?php

try {
    request_parse_body(options: [
        'upload_max_filesize' => '1GB',
    ]);
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Invalid quantity "1GB": unknown multiplier "B", interpreting as "1" for backwards compatibility in %s on line %d
Error: Request does not provide a content type
