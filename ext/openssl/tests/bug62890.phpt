--TEST--
Bug #62890 (default_socket_timeout=-1 causes connection to timeout)
--SKIPIF--
<?php
if (!extension_loaded('openssl')) die('skip openssl extension not available');
if (getenv('SKIP_ONLINE_TESTS')) die('skip online test');
?>
--INI--
default_socket_timeout=-1
--FILE--
<?php
$clientCtx = stream_context_create(['ssl' => ['verify_peer' => false]]);
var_dump((bool) file_get_contents('https://www.php.net', false, $clientCtx));
?>
--EXPECT--
bool(true)
