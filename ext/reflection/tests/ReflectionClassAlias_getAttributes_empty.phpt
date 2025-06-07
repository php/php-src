--TEST--
ReflectionClassAlias::__toString()
--FILE--
<?php

class_alias( 'ReflectionClassAlias', 'MyReflectionClassAlias' );

$r = new ReflectionClassAlias( 'MyReflectionClassAlias' );
var_dump( $r->getAttributes() );

?>
--EXPECT--
array(0) {
}
