--TEST--
SCCP 035: memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
  $obj = new stdClass;
  $obj->$b = ~$b = $a='';
  $obj->$a--;
}
?>
DONE
--EXPECT--
Deprecated: Decrement on empty string is deprecated as non-numeric in Unknown on line 0
DONE
