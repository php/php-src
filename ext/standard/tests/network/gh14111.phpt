--TEST--
Testing linger `socket` option.
--SKIPIF--
<?php
//if (getenv("SKIP_ONLINE_TESTS")) die('skip online test');
if (!in_array('https', stream_get_wrappers())) die('skip: https wrapper is required');
?>
--FILE--
<?php
$context = stream_context_create(['socket' => ['linger' => false]]);
var_dump(file_get_contents('https://httpbin.org/get', false, $context) !== false);
$context = stream_context_create(['socket' => ['linger' => PHP_INT_MAX + 1]]);
var_dump(file_get_contents('https://httpbin.org/get', false, $context) !== false);
$context = stream_context_create(['socket' => ['linger' => 3]]);
var_dump(file_get_contents('https://httpbin.org/get', false, $context) !== false);
?>
--EXPECTF--
Warning: file_get_contents(https://httpbin.org/get): Failed to open stream: Invalid `linger` value in %s on line %d
bool(false)

Warning: file_get_contents(https://httpbin.org/get): Failed to open stream: Invalid `linger` value in %s on line %d
bool(false)
bool(true)
