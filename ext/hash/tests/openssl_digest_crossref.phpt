--TEST--
Hash: Cross-reference against OpenSSL
--EXTENSIONS--
openssl
--FILE--
<?php
echo "Verifying hash() against OpenSSL with supported algorithms ...\n";
$pass = true;
foreach (array_intersect(hash_algos(), openssl_get_md_methods()) as $algo) {
	$hash = hash($algo, $algo);
	$ossl = openssl_digest($algo, $algo);
	// OpenSSL engines can be disabled outside of PHP
	if ($ossl === false) {
		continue;
	} elseif ($hash !== $ossl) {
		echo "{$algo} PHP:     {$hash}\n";
		echo "{$algo} OpenSSL: {$ossl}\n";
		$pass = false;
	}
}

echo ($pass === true) ? "OK!\n" : "FAILED!\n"; 
?>
--EXPECT--
Verifying hash() against OpenSSL with supported algorithms ...
OK!
