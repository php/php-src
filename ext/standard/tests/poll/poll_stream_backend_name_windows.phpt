--TEST--
Poll stream - backend name on Windows
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die ("skip only for Windows");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';
// this just prints default poll ctx
$poll_ctx = new PollContext();
var_dump($poll_ctx->getBackend());
// test with WSAPoll
$poll_ctx = new PollContext(PollBackend::WSAPoll);
$backend = $poll_ctx->getBackend();
var_dump($backend->name);
var_dump($backend->value);
?>
--EXPECTF--
enum(PollBackend::%s)
string(7) "WSAPoll"
string(7) "wsapoll"
