--TEST--
Bug #21039
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	error_reporting(E_ALL);
    $nthash    = "00000000000000000";
    $challenge = "foo";
    $td = mcrypt_module_open ('des', '', 'cbc', '');
    $iv = mcrypt_create_iv (mcrypt_enc_get_iv_size ($td), MCRYPT_RAND);
    $var = @mcrypt_generic_init ($td, substr($nthash, 0, 8));
    $res = @mcrypt_generic ($td, $challenge);
    print "I'm alive!\n";
?>
--EXPECT--
I'm alive!
