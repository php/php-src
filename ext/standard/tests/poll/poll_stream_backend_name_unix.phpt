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
$poll_ctx = new Io\Poll\Context();
var_dump($poll_ctx->getBackend());
// test with poll that is always available on Unix systems
$poll_ctx = new Io\Poll\Context(Io\Poll\Backend::Poll);
$backend = $poll_ctx->getBackend();
var_dump($backend->name);
?>
--EXPECTF--
enum(Io\Poll\Backend::%s)
string(4) "Poll"
