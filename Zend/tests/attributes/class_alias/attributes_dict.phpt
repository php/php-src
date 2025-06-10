--TEST--
Alias attributes must not be associative
--FILE--
<?php

#[ClassAlias('Other', ['test' => new Deprecated()])]
class Demo {}

class_alias( 'Demo', 'Other2' );

$attr = new ReflectionClass( Demo::class )->getAttributes()[0];
$attr->newInstance();

?>
--EXPECTF--
Fatal error: Uncaught Error: ClassAlias::__construct(): Argument #2 ($attributes) must be a list, not an associative array in %s:%d
Stack trace:
#0 %s(%d): ClassAlias->__construct('Other', Array)
#1 %s(%d): ReflectionAttribute->newInstance()
#2 {main}
  thrown in %s on line %d
