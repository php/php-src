--TEST--
User streams and incrrectly typed context
--FILE--
<?php
class Wrapper {
    public Foo $context;
}
if (stream_wrapper_register('foo', Wrapper::class)) dir('foo://');
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign resource to property Wrapper::$context of type Foo in %suser_streams_context_001.php:5
Stack trace:
#0 %suser_streams_context_001.php(5): dir('foo://')
#1 {main}
  thrown in %suser_streams_context_001.php on line 5
