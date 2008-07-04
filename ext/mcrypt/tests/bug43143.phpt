--TEST--
Bug #43143 (Warning about empty IV with MCRYPT_MODE_ECB)
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
echo "ECB\n";
$input = 'to be encrypted';
$mkey = hash('sha256', 'secret key', TRUE);
$data = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $mkey, $input, MCRYPT_MODE_ECB);
echo "CFB\n";
$input = 'to be encrypted';
$mkey = hash('sha256', 'secret key', TRUE);
$data = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $mkey, $input, MCRYPT_MODE_CFB);
echo "END\n";
?>
--EXPECTF--
ECB
CFB

Warning: mcrypt_encrypt(): Attempt to use an empty IV, which is NOT recommend in %sbug43143.php on line 9
END
