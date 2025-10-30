--TEST--
Poll stream - backend name
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';
// this just prints default poll ctx
$poll_ctx = new PollContext();
var_dump($poll_ctx->getBackend());
// test with poll that is always available on Unix systems
$poll_ctx = new PollContext(PollBackend::Poll);
$backend = $poll_ctx->getBackend();
var_dump($backend->name);
var_dump($backend->value);

?>
--EXPECTF--
enum(PollBackend::%s)
string(4) "Poll"
string(4) "poll"
