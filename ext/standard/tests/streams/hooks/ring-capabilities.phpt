--TEST--
Io\Ring\Engine: no hook capabilities by default, Files and Direct only on request
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
$ring = new Io\Ring\Engine();
var_dump($ring->getHookCapabilities());
var_dump((new Io\Poll\OperationQueue())->getHookCapabilities());

// Direct where the backend completes ops itself: io_uring and IOCP, not the thread pool
$expected = $ring->getBackend() === Io\Ring\Backend::Threads
    ? [Io\Hooks\Capability::Files]
    : [Io\Hooks\Capability::Files, Io\Hooks\Capability::Direct];
var_dump($ring->getSupportedHookCapabilities() === $expected);
?>
--EXPECT--
array(0) {
}
array(0) {
}
bool(true)
