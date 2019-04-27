--TEST--
Bug #71523 (Copied handle with new option CURLOPT_HTTPHEADER crashes while curl_multi_exec)
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$base = curl_init('http://www.google.com/');
curl_setopt($base, CURLOPT_RETURNTRANSFER, true);
$mh = curl_multi_init();

for ($i = 0; $i < 2; ++$i) {
	$ch = curl_copy_handle($base);
	curl_setopt($ch, CURLOPT_HTTPHEADER, ['Foo: Bar']);
	curl_multi_add_handle($mh, $ch);
}

do {
	curl_multi_exec($mh, $active);
} while ($active);
?>
okey
--EXPECT--
okey
