--TEST--
ReflectionClassAlias::__toString()
--FILE--
<?php

class_alias( 'ReflectionClassAlias', 'MyReflectionClassAlias' );

$r = new ReflectionClassAlias( 'MyReflectionClassAlias' );
echo $r;
?>
--EXPECT--
MyReflectionClassAlias - alias for ReflectionClassAlias
