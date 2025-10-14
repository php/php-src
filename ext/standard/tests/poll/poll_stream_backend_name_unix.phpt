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
// poll is always available on Unix systems
$poll_ctx = new PollContext(POLL_BACKEND_POLL);
var_dump($poll_ctx->getBackendName());
// test with string
$poll_ctx = new PollContext('poll');
var_dump($poll_ctx->getBackendName());

?>
--EXPECT--
string(4) "poll"
string(4) "poll"
