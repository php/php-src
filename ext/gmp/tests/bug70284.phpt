--TEST--
Bug #70284 (Use after free vulnerability in unserialize() with GMP)
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$inner = 'r:2;a:1:{i:0;a:1:{i:0;r:4;}}';
$exploit = 'a:2:{i:0;s:1:"1";i:1;C:3:"GMP":'.strlen($inner).':{'.$inner.'}}';

$data = unserialize($exploit);

$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";

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
Fatal error: Uncaught Exception: Could not unserialize number in %sbug70284.php:6
Stack trace:
#0 %sbug70284.php(6): unserialize('%s')
#1 {main}
  thrown in %sbug70284.php on line 6
