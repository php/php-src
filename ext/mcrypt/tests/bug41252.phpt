--TEST--
Bug #41252 (Calling mcrypt_generic without first calling mcrypt_generic_init crashes)
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open(MCRYPT_DES, '', MCRYPT_MODE_ECB, '');
echo mcrypt_generic($td,'aaaaaaaa');
print "I'm alive!\n";
?>
--EXPECTF--
Warning: mcrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug41252.php on line 3
I'm alive!
