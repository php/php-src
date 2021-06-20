--TEST--
Reference to SplObjectStorage key (not supported)
--FILE--
<?php

$inner = 'x:i:1;O:8:"stdClass":0:{};m:a:0:{}';
$inner_len = strlen($inner);
$str = <<<STR
a:2:{i:0;C:16:"SPlObjectStorage":{$inner_len}:{{$inner}}i:1;R:4;}
STR;
var_dump(unserialize($str));

?>
--EXPECT--
array(2) {
  [0]=>
  object(SplObjectStorage)#1 (1) {
    ["storage":"SplObjectStorage":private]=>
    array(1) {
      [0]=>
      array(2) {
        ["obj"]=>
        object(stdClass)#2 (0) {
        }
        ["inf"]=>
        NULL
      }
    }
  }
  [1]=>
  object(stdClass)#2 (0) {
  }
}
