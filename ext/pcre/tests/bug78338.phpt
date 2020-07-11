--TEST--
Bug #78338 (Array cross-border reading in PCRE)
--INI--
pcre.jit=1
--FILE--
<?php
$string = hex2bin('2f5c583f3d3f223f3536ff3636ffffffff36a8a8a83636367a7a7a7a3d2aff2f0a');
preg_match($string, $string);
echo "DONE\n";
?>
--EXPECT--
DONE
