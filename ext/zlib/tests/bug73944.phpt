--TEST--
Bug #73944: Dictionary option of intflate_init() does not work
--EXTENSIONS--
zlib
--FILE--
<?php

$in = inflate_init(ZLIB_ENCODING_RAW, array('dictionary' => str_repeat("\00", 32768)));
$a = inflate_add($in, file_get_contents(__DIR__.'/bug73944_fixture_1.data'));
echo '1 block: '.strlen($a).PHP_EOL;

$in = inflate_init(ZLIB_ENCODING_RAW, array('dictionary' => $a));
$b = inflate_add($in, file_get_contents(__DIR__.'/bug73944_fixture_2.data'));
echo '2 block: '.($b === false ? 'failed' : strlen($b)).PHP_EOL;

?>
--EXPECT--
1 block: 32768
2 block: 32768
