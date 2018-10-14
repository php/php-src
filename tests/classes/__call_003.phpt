--TEST--
Force pass-by-reference to __call
--FILE--
<?php
  class C
  {
      function __call($name, $values)
      {
          $values[0][0] = 'changed';
      }
  }

  $a = array('original');

  $b = array('original');
  $hack =& $b[0];

  $c = new C;
  $c->f($a);
  $c->f($b);

  var_dump($a, $b);
?>
--EXPECTF--
array(1) {
  [0]=>
  string(8) "original"
}
array(1) {
  [0]=>
  &string(7) "changed"
}
