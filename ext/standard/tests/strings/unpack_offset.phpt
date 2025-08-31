--TEST--
unpack() with offset
--FILE--
<?php
$data = "pad" . pack("ll", 0x01020304, 0x05060708);

$a = unpack("l2", $data, 3);
printf("0x%08x 0x%08x\n", $a[1], $a[2]);

printf("0x%08x 0x%08x\n",
    unpack("l", $data, 3)[1],
    unpack("@4/l", $data, 3)[1]);

try {
    unpack("l", "foo", 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    unpack("l", "foo", -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
0x01020304 0x05060708
0x01020304 0x05060708
unpack(): Argument #3 ($offset) must be contained in argument #2 ($data)
unpack(): Argument #3 ($offset) must be contained in argument #2 ($data)
