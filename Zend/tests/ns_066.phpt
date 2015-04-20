--TEST--
066: Name ambiguity (import name & internal class name)
--FILE--
<?php
use Foo\Bar\Foo as stdClass;
include __DIR__ . '/ns_027.inc';

new stdClass();
--EXPECT--
Foo\Bar\Foo
