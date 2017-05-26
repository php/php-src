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

$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";

$inner = 'r:2;';
$exploit = 'a:2:{i:0;i:1;i:1;C:3:"obj":'.strlen($inner).':{'.$inner.'}}';

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
array(2) {
  [0]=>
  int(1)
  [1]=>
  object(obj)#%d (1) {
    ["data"]=>
    int(1)
  }
}
