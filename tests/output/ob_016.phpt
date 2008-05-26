--TEST--
output buffering - unicode/binary
--FILE--
<?php
declare(encoding="latin1");
ini_set("unicode.output_encoding", "utf8");

ob_start();
echo "-aäoöuüsß-";
$s = ob_get_clean();

var_dump($s);
var_dump(unicode_decode($s, "utf8"));
?>
--EXPECTF--
string(14) "%s"
unicode(10) "%s"
