--TEST--
GH-15937 (stream overflow on timeout setting)
--SKIPIF--
<?php if (getenv("SKIP_ONLINE_TESTS")) die("skip online test"); ?>
--FILE--
<?php
$config = [
    'http' => [
        'timeout' => PHP_INT_MAX,
    ],
];
$ctx = stream_context_create($config);
var_dump(fopen("http://www.example.com", "r", false, $ctx));
?>
--EXPECTF--
resource(%d) of type (stream)
