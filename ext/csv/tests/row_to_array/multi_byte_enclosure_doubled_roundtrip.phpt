--TEST--
Test Csv\row_to_array(): field containing a multibyte enclosure sequence round-trips
--EXTENSIONS--
csv
--FILE--
<?php
$row = Csv\array_to_row(['aaa'], ',', 'aa', "\n");
echo json_encode($row), \PHP_EOL;
echo json_encode(Csv\row_to_array($row, ',', 'aa', "\n")), \PHP_EOL;

$row = Csv\array_to_row(['x', 'y--z', 'w'], ',', '--', "\n");
echo json_encode($row), \PHP_EOL;
echo json_encode(Csv\row_to_array($row, ',', '--', "\n")), \PHP_EOL;
?>
--EXPECT--
"aaaaaaaaa\n"
["aaa"]
"x,--y----z--,w\n"
["x","y--z","w"]
