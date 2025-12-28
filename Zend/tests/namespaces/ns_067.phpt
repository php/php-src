--TEST--
067: Name ambiguity (import name & internal class name)
--FILE--
<?php
include __DIR__ . '/ns_022.inc';
include __DIR__ . '/ns_027.inc';
include __DIR__ . '/ns_067.inc';
?>
--EXPECT--
Foo\Bar\Foo
