--TEST--
GH-19953 (memory leak on unimeplemented callback for custom stream scheme)
--CREDITS--
YuanchengJiang
--FILE--
<?php
class DummyStreamWrapper
{
public function stream_open(string $path, string $mode, int $options = 0, ?string &$openedPath = null): bool
{
return true;
}
}
class TrampolineTest {
public function __call(string $name, array $arguments) {
if (!$this->wrapper) {
$this->wrapper = new DummyStreamWrapper();
}
return $this->wrapper->$name(...$arguments);
}
}
stream_wrapper_register('custom', TrampolineTest::class);
$fp = fopen("custom://myvar", "r+");
?>
--EXPECTF--
Deprecated: Creation of dynamic property TrampolineTest::$context is deprecated in %s on line %d

Warning: Undefined property: TrampolineTest::$wrapper in %s on line %d

Deprecated: Creation of dynamic property TrampolineTest::$wrapper is deprecated in %s on line %d

Fatal error: Uncaught Error: Call to undefined method DummyStreamWrapper::stream_close() in %s:%d
Stack trace:
#0 [internal function]: TrampolineTest->__call('stream_close', Array)
#1 {main}
  thrown in %s on line %d
