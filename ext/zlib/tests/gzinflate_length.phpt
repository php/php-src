--TEST--
gzinflate() and $length argument
--EXTENSIONS--
zlib
--FILE--
<?php
$original = 'aaaaaaaaaaaaaaa';
$packed=gzdeflate($original);
echo strlen($packed)." ".strlen($original)."\n";
$unpacked=gzinflate($packed, strlen($original));
if (strcmp($original,$unpacked)==0) echo "Strings are equal\n";

$unpacked=gzinflate($packed, strlen($original)*10);
if (strcmp($original,$unpacked)==0) echo "Strings are equal\n";

try {
    $unpacked=gzinflate($packed, 1);
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
5 15
Strings are equal
Strings are equal
insufficient memory
