--TEST--
Bug #73944: Dictionary option of intflate_init() does not work
--FILE--
<?php

$in = inflate_init(ZLIB_ENCODING_RAW, array('dictionary' => str_repeat("\00", 32768)));
$a = inflate_add($in, file_get_contents(__DIR__.'/bug73944_fixture1'));
echo '1 block: '.strlen($a).PHP_EOL;

$in = inflate_init(ZLIB_ENCODING_RAW, array('dictionary' => $a));
$b = inflate_add($in, file_get_contents(__DIR__.'/bug73944_fixture2'));
echo '2 block: '.($b === false ? 'failed' : strlen($b)).PHP_EOL;

?>
--EXPECTF--
1 block: 32768
2 block: 32768
