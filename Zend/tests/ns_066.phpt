--TEST--
066: Name ambiguity (import name & internal class name)
--FILE--
<?php
include __DIR__ . '/ns_027.inc';
use Foo\Bar\Foo as StdClass;

new StdClass();
--EXPECT--
Foo\Bar\Foo
