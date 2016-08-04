--TEST--
Bug #72562: Use After Free in unserialize() with Unexpected Session Deserialization
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ini_set('session.serialize_handler', 'php_binary');
session_start();
$sess = "\x1xi:1;\x2y";
session_decode($sess);
$uns_1 = '{';
$out_1[] = unserialize($uns_1);
unset($out_1);
$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";
for ($i = 0; $i < 5; $i++) {
  $v[$i] = $fakezval.$i;
}
$uns_2 = 'R:2;';
$out_2 = unserialize($uns_2);
var_dump($out_2);

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
Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s%ebug72562.php on line %d

Notice: unserialize(): Error at offset 0 of 1 bytes in %s%ebug72562.php on line %d

Notice: unserialize(): Error at offset 4 of 4 bytes in %s%ebug72562.php on line %d
bool(false)
