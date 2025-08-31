--TEST--
Bug #70172 - Use After Free Vulnerability in unserialize()
--FILE--
<?php
class obj implements Serializable {
    var $data;
    function serialize() {
        return serialize($this->data);
    }
    function unserialize($data) {
        $this->data = unserialize($data);
    }
}

class obj2 {
    var $ryat;
    function __wakeup() {
        $this->ryat = 1;
    }
}

$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";

$inner = 'r:2;';
$exploit = 'a:2:{i:0;O:4:"obj2":1:{s:4:"ryat";C:3:"obj":'.strlen($inner).':{'.$inner.'}}i:1;a:1:{i:0;a:1:{i:0;R:4;}}}';

$data = unserialize($exploit);

for ($i = 0; $i < 5; $i++) {
    $v[$i] = $fakezval.$i;
}

var_dump($data);

function ptr2str($ptr)
{
    $out = '';
    for ($i = 0; $i < 8; $i++) {
        $out .= chr($ptr & 0xff);
        $ptr >>= 8;
    }
    return $out;
}
?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
array(2) {
  [0]=>
  object(obj2)#%d (1) {
    ["ryat"]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      object(obj2)#%d (1) {
        ["ryat"]=>
        int(1)
      }
    }
  }
}
