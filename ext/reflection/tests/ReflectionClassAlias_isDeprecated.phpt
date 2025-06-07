--TEST--
ReflectionClassAlias::isDeprecated()
--FILE--
<?php

#[ClassAlias('OldAlias', [new Deprecated()])]
#[ClassAlias('NewAlias')]
class Demo {}

$r = new ReflectionClassAlias( 'OldAlias' );
var_dump( $r->isDeprecated() );

$r = new ReflectionClassAlias( 'NewAlias' );
var_dump( $r->isDeprecated() );
?>
--EXPECT--
bool(true)
bool(false)
