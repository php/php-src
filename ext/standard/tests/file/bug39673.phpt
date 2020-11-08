--TEST--
Bug #39673 (file_get_contents causes bus error on certain offsets)
--FILE--
<?php

$str = str_repeat("test", 3456);

$filename = __DIR__.'/bug39673.txt';
file_put_contents($filename, $str);

$offsets = array(
    -1,
    0,
    3456*4,
    3456*4 - 1,
    3456*4 + 1,
    2000,
    5000,
    100000,
);


foreach ($offsets as $offset) {
    $r = file_get_contents($filename, false, null, $offset);
    if ($r !== false) var_dump(strlen($r));
}

@unlink($filename);
echo "Done\n";
?>
--EXPECT--
int(1)
int(13824)
int(0)
int(1)
int(0)
int(11824)
int(8824)
int(0)
Done
