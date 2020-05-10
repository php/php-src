--TEST--
Allow passing extension $self as reference
--FILE--
<?php

use extension array ArrayExtension;

class ArrayExtension {
    public static function append($self, $value) {
        $self[] = $value;
    }

    public static function appendByRef(&$self, $value) {
        $self[] = $value;
    }
}

$x = [];
$x->append('foo');
var_dump($x);
$x->appendByRef('foo');
var_dump($x);

?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  string(3) "foo"
}
