--TEST--
Bug #46108 (DateTime - Memory leak when unserializing)
--FILE--
<?php

date_default_timezone_set('America/Sao_Paulo');

var_dump(unserialize(serialize(new Datetime)));

?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(%d)
  ["timezone"]=>
  string(%d) "America/Sao_Paulo"
}
