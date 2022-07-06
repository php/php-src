--TEST--
Bug #79199 (curl_copy_handle() memory leak)
--SKIPIF--
<?php
if (!extension_loaded('curl')) die('skip curl extension not available');
?>
--FILE--
<?php
$mem_old = 0;
for($i = 0; $i < 50; ++$i) {
    $c1 = curl_init();
    $c2 = curl_copy_handle($c1);
    curl_close($c2);
    curl_close($c1);
    $mem_new = memory_get_usage();
    if ($mem_new <= $mem_old) {
        break;
    }
    $mem_old = $mem_new;
}
echo $i < 50 ? "okay" : "leak", PHP_EOL;
?>
--EXPECT--
okay
