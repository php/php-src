--TEST--
ReflectionClassAlias::__toString() for a deprecated alias
--FILE--
<?php

#[ClassAlias('Other', [new Deprecated()])]
class Demo {}

$r = new ReflectionClassAlias( 'Other' );
echo $r;
?>
--EXPECT--
Other - deprecated alias for Demo
