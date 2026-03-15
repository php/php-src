--TEST--
Poll context - backend on Windows
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
$poll_ctx = new Io\Poll\Context();
var_dump($poll_ctx->getBackend());
// test with WSAPoll
$poll_ctx = new Io\Poll\Context(Io\Poll\Backend::WSAPoll);
$backend = $poll_ctx->getBackend();
var_dump($backend->name);
try {
    new Io\Poll\Context(Io\Poll\Backend::Epoll);
} catch (\Io\Poll\BackendUnavailableException $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
enum(Io\Poll\Backend::%s)
string(7) "WSAPoll"
string(27) "Backend Epoll not available"
