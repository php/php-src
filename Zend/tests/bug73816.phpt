--TEST--
Bug #73816: Broken eval(anonymous class)
--FILE--
<?php

function anon()
{
    static $i = 0;
    return eval(sprintf('return new class { private $prop%s; };', ++$i));
}

var_dump(anon());
var_dump(anon());

?>
--EXPECT--
object(class@anonymous)#1 (1) {
  ["prop1":"class@anonymous":private]=>
  NULL
}
object(class@anonymous)#1 (1) {
  ["prop2":"class@anonymous":private]=>
  NULL
}
