--TEST--
Bug #79265 variation: "host:" not at start of header
--INI--
allow_url_fopen=1
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
$uri = "http://www.example.com";
$config = [
'http' => [
'timeout' => PHP_INT_MIN,
],
];
$ctx = stream_context_create($config);
var_dump(fopen($uri, "r", false, $ctx));

$config['http']['timeout'] = PHP_INT_MAX;
$ctx = stream_context_create($config);
var_dump(fopen($uri, "r", false, $ctx));
?>
--EXPECTF--
resource(%d) of type (stream)

Warning: fopen(http://www.example.com): Failed to open stream: timeout must be lower than %d in %s on line %d
bool(false)
