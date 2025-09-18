--TEST--
Bug #68976 Use After Free Vulnerability in unserialize()
--FILE--
<?php
class evilClass {
    public $name;
    function __wakeup() {
        unset($this->name);
    }
}

$fakezval = pack(
    'IIII',
    0x00100000,
    0x00000400,
    0x00000000,
    0x00000006
);

$data = unserialize('a:2:{i:0;O:9:"evilClass":1:{s:4:"name";a:2:{i:0;i:1;i:1;i:2;}}i:1;R:4;}');

for($i = 0; $i < 5; $i++) {
    $v[$i] = $fakezval.$i;
}

var_dump($data);
?>
--EXPECTF--
Deprecated: The __wakeup() serialization magic method has been deprecated. Implement __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
array(2) {
  [0]=>
  object(evilClass)#1 (0) {
  }
  [1]=>
  int(1)
}
