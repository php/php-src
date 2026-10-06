--TEST--
Unknown CDF property IDs are printed with the 0x prefix
--EXTENSIONS--
fileinfo
--FILE--
<?php
$data = file_get_contents(__DIR__ . '/resources/test.ppt');
$data[40576] = "\x1f";
$desc = (new finfo())->buffer($data);
echo substr($desc, strrpos($desc, ', ') + 2), "\n";
?>
--EXPECT--
0x1f: 0
