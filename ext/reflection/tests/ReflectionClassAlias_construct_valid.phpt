--TEST--
ReflectionClassAlias::__construct() - with an alias
--FILE--
<?php

class_alias( 'ReflectionClassAlias', 'MyReflectionClassAlias' );

$r = new ReflectionClassAlias( 'MyReflectionClassAlias' );
var_dump( $r );
?>
--EXPECTF--
object(ReflectionClassAlias)#%d (1) {
  ["name"]=>
  string(22) "MyReflectionClassAlias"
}
