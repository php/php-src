--TEST--
Io\Ring\Engine: EdgeRegistrations by default, Files, DirectData and DirectAccept on request
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
$ring = new Io\Ring\Engine();
var_dump($ring->getHookCapabilities());
var_dump((new Io\Poll\OperationQueue())->getHookCapabilities());

// DirectData where the backend completes ops itself: io_uring and IOCP, not the thread pool;
// DirectAccept everywhere, from a multishot accept
$expected = $ring->getBackend() === Io\Ring\Backend::Threads
    ? [Io\Hooks\Capability::Files, Io\Hooks\Capability::DirectAccept, Io\Hooks\Capability::EdgeRegistrations]
    : [Io\Hooks\Capability::Files, Io\Hooks\Capability::DirectData, Io\Hooks\Capability::DirectAccept,
        Io\Hooks\Capability::EdgeRegistrations];
var_dump($ring->getSupportedHookCapabilities() === $expected);
?>
--EXPECT--
array(1) {
  [0]=>
  enum(Io\Hooks\Capability::EdgeRegistrations)
}
array(2) {
  [0]=>
  enum(Io\Hooks\Capability::EdgeRegistrations)
  [1]=>
  enum(Io\Hooks\Capability::LevelRegistrations)
}
bool(true)
