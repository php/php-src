--TEST--
Bug #12190 (Setting 0 with port 0 too)
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die('skip online test');
if (!in_array('https', stream_get_wrappers())) die('skip: https wrapper is required');
?>
--FILE--
<?php
$context = stream_context_create(['socket' => ['bindto' => '0:0']]);
var_dump(file_get_contents('https://httpbin.org/get', false, $context) !== false);
?>
--EXPECT--
bool(true)
