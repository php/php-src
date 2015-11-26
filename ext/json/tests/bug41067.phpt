--TEST--
Bug #41067 (json_encode() problem with UTF-16 input)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$single_barline = "\360\235\204\200";
$array = array($single_barline);
print bin2hex($single_barline) . "\n";
// print $single_barline . "\n\n";
$json = json_encode($array);
print $json . "\n\n";
$json_decoded = json_decode($json, true);
// print $json_decoded[0] . "\n";
print bin2hex($json_decoded[0]) . "\n";
print "END\n";
?>
--EXPECT--
f09d8480
["\ud834\udd00"]

f09d8480
END
