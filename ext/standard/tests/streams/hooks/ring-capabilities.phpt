--TEST--
Io\Ring\Engine: EdgeRegistrations and DirectAccept by default, Files and DirectData on request
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
$ring = new Io\Ring\Engine();
// Edge registrations where the multishot poll reports edges: not on IOCP, whose poll re-reports
// readiness that persists
$edge = $ring->getBackend() !== Io\Ring\Backend::Iocp;
$default = [Io\Hooks\Capability::DirectAccept];
if ($edge) {
    $default[] = Io\Hooks\Capability::EdgeRegistrations;
}
var_dump($ring->getHookCapabilities() === $default);
var_dump((new Io\Poll\OperationQueue())->getHookCapabilities());

// DirectData where the backend completes ops itself: io_uring and IOCP, not the thread pool;
// DirectAccept everywhere, from a multishot accept
$expected = [Io\Hooks\Capability::Files];
if ($ring->getBackend() !== Io\Ring\Backend::Threads) {
    $expected[] = Io\Hooks\Capability::DirectData;
}
$expected[] = Io\Hooks\Capability::DirectAccept;
if ($edge) {
    $expected[] = Io\Hooks\Capability::EdgeRegistrations;
}
var_dump($ring->getSupportedHookCapabilities() === $expected);
?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  enum(Io\Hooks\Capability::EdgeRegistrations)
  [1]=>
  enum(Io\Hooks\Capability::LevelRegistrations)
}
bool(true)
