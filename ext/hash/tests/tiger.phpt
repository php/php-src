--TEST--
tiger
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('tiger192,3', ''),"\n";
echo hash('tiger192,3', 'abc'),"\n";
echo hash('tiger192,3', str_repeat('a', 63)),"\n";
echo hash('tiger192,3', str_repeat('abc', 61)),"\n";
echo hash('tiger192,3', str_repeat('abc', 64)),"\n";
?>
--EXPECT--
24f0130c63ac933216166e76b1bb925ff373de2d49584e7a
f258c1e88414ab2a527ab541ffc5b8bf935f7b951c132951
8ee409a14e6066933b63d5b2abca63d71a78f55e29eb4649
2586156d16bf9ab1e6e48bdf5e038f8053c30e071db3bcb0
3ee8a9405396ddba1bc038508af4164ac1fe59ef58916a85
