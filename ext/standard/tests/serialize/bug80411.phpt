--TEST--
Bug #80411: References to null-serialized object break serialize()
--FILE--
<?php

class UnSerializable implements Serializable
{
  public function serialize() {}
  public function unserialize($serialized) {}
}

$unser = new UnSerializable();
$arr = [$unser];
$arr[1] = &$arr[0];
$arr[2] = 'endcap';
$arr[3] = &$arr[2];

$data = serialize($arr);
echo $data . PHP_EOL;
$recovered = unserialize($data);
var_export($recovered);

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
a:4:{i:0;N;i:1;N;i:2;s:6:"endcap";i:3;R:4;}
array (
  0 => NULL,
  1 => NULL,
  2 => 'endcap',
  3 => 'endcap',
)
