--TEST--
curl_exec() on one handle with and without an IO hooks provider
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$host = curl_cli_server_start();

$ch = curl_init("{$host}/get.inc?test=get");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

var_dump(curl_exec($ch));

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);
$scheduler->spawn(function () use ($ch) {
    var_dump(curl_exec($ch));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);

var_dump(curl_exec($ch));
var_dump(curl_upkeep($ch));
?>
--EXPECT--
string(25) "Hello World!
Hello World!"
string(25) "Hello World!
Hello World!"
string(25) "Hello World!
Hello World!"
bool(true)
