--TEST--
mcrypt basic
--FILE--
<?php
$as = mcrypt_list_algorithms();
shuffle($as);
$ms = mcrypt_list_modes();
shuffle($ms);
/* test only 5 algos with 3 modes, so we don't timeout */
foreach (array_slice($as, 0, 5) as $a) {
	foreach (array_slice($ms, 0, 3) as $m) {
		if ((mcrypt_module_is_block_algorithm($a) == mcrypt_module_is_block_algorithm_mode($m)) && ($is = mcrypt_get_iv_size($a, $m))) {
			$iv = mcrypt_create_iv($is);
			$rs = mcrypt_decrypt($a, b"key", mcrypt_encrypt($a, b"key", b"data", $m, $iv), $m, $iv);
			if (b"data" !== (mcrypt_module_is_block_mode($m) ? rtrim($rs, b"\0") : $rs)) {
				echo "FAIL: $a/$m "; var_dump($rs);
			}
		}
	}
}
?>
--EXPECT--
