--TEST--
GH-15937 (stream overflow on timeout setting)
--FILE--
<?php
$config = [
    'http' => [
        'timeout'		=> PHP_INT_MAX,
    ],
];
$ctx = stream_context_create($config);
var_dump(fopen("http://example.com", "r", false, $ctx));
?>
--EXPECTF--
resource(%d) of type (stream)
