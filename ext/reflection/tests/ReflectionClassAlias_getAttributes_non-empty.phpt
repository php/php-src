--TEST--
ReflectionClassAlias::__toString()
--FILE--
<?php

#[ClassAlias('MyAlias', [new MissingAttribute()])]
class Demo {}

$r = new ReflectionClassAlias( 'MyAlias' );
var_dump( $r->getAttributes() );

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(16) "MissingAttribute"
  }
}
