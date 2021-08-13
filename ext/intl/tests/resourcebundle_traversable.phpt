--TEST--
Bug #55610: ResourceBundle does not implement Traversable
--EXTENSIONS--
intl
--FILE--
<?php
    include "resourcebundle.inc";

    $r = new ResourceBundle( 'es', BUNDLE );

    var_dump($r instanceof Traversable);
    var_dump(iterator_to_array($r->get('testarray')));
?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  string(8) "cadena 1"
  [1]=>
  string(8) "cadena 2"
  [2]=>
  string(8) "cadena 3"
}
