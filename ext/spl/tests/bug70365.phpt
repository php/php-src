--TEST--
SPL: Bug #70365 use-after-free vulnerability in unserialize() with SplObjectStorage
--FILE--
<?php
class obj {
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

$inner = 'x:i:1;O:8:"stdClass":0:{},i:1;;m:a:0:{}';
$exploit = 'a:5:{i:0;i:1;i:1;C:16:"SplObjectStorage":'.strlen($inner).':{'.$inner.'}i:2;O:3:"obj":1:{s:4:"ryat";R:3;}i:3;R:6;i:4;s:'.strlen($fakezval).':"'.$fakezval.'";}';

$data = unserialize($exploit);

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
--EXPECTF--
array(5) {
  [0]=>
  int(1)
  [1]=>
  &int(1)
  [2]=>
  object(obj)#%d (1) {
    ["ryat"]=>
    &int(1)
  }
  [3]=>
  int(1)
  [4]=>
  string(24) "%s"
}
