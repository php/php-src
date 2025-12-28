--TEST--
GH-18322 (SplObjectStorage debug handler mismanages memory)
--FILE--
<?php

$stor = new SplObjectStorage();
$obj = new stdClass;
$stor[$obj] = 1;

$tmp = $stor->__debugInfo();
$tmp2 = $tmp[array_key_first($tmp)];
unset($tmp); // Drop $tmp2 RC to 1
$tmp2[0]['obj'] = new stdClass;
var_dump($tmp2);

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["obj"]=>
    object(stdClass)#3 (0) {
    }
    ["inf"]=>
    int(1)
  }
}
