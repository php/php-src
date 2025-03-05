--TEST--
#[\NonpublicConstructor]: affects message from ReflectionClass::newInstance()
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor("use ::getInstance() instead")]
    private function __construct( $unused ) {}
}

$reflection = new ReflectionClass( Demo::class );
$reflection->newInstance( true );

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Access to non-public constructor of class Demo, use ::getInstance() instead, in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstance(true)
#1 {main}
  thrown in %s on line %d
