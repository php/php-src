--TEST--
Bug #53437 (DateInterval unserialize bad data, 64 bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) { die('skip true 64 bit only'); } ?>
--FILE--
<?php
$s = 'O:12:"DateInterval":8:{s:1:"y";s:1:"2";s:1:"m";s:1:"0";s:1:"d";s:3:"bla";s:1:"h";s:1:"6";s:1:"i";s:1:"8";s:1:"s";s:1:"0";s:6:"invert";i:0;s:4:"days";s:4:"aoeu";}';

$di = unserialize($s);
var_dump($di);

?>
--EXPECTF--
object(DateInterval)#%d (%d) {
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
  ["from_string"]=>
  bool(false)
}
