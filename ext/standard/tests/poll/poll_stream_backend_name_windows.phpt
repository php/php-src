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
// wsapoll is always available on Windows
$poll_ctx = new PollContext(POLL_BACKEND_WSAPOLL);
var_dump($poll_ctx->getBackendName());
// test with string
$poll_ctx = new PollContext('wsapoll');
var_dump($poll_ctx->getBackendName());

?>
--EXPECT--
string(7) "wsapoll"
string(7) "wsapoll"
