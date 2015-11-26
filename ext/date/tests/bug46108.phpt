--TEST--
Bug #46108 (DateTime - Memory leak when unserializing)
--FILE--
<?php

date_default_timezone_set('America/Sao_Paulo');

var_dump(unserialize(serialize(new Datetime)));

?>
--EXPECTF--
object(DateTime)#%d (3) {
  [%u|b%"date"]=>
  %string|unicode%(%d) "%s"
  [%u|b%"timezone_type"]=>
  int(%d)
  [%u|b%"timezone"]=>
  %string|unicode%(%d) "America/Sao_Paulo"
}
