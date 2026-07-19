--TEST--
Bug#74951 Null pointer dereference in user streams
--FILE--
<?php
trait Stream00ploiter{
  public function s() {}
  public function n($_) {}
}

try {
	stream_wrapper_register('e0ploit','Stream00ploiter');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
ValueError: stream_wrapper_register(): Argument #2 ($class) must be a concrete class
