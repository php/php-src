--TEST--
request_parse_body: reference in options array
--FILE--
<?php
$options = ['post_max_size' => '128M'];
foreach ($options as $k => &$v) {}
try {
    request_parse_body($options);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Request does not provide a content type
