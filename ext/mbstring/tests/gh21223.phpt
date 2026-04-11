--TEST--
GH-21223 (Stack overflow in mb_guess_encoding called via mb_detect_encoding)
--EXTENSIONS--
mbstring
--FILE--
<?php
$str = "hello";

$list = [];
for ($i = 0; $i < 500000; $i++) {
    $list[] = "UTF-8";
}

var_dump(mb_detect_encoding($str, $list, false));
echo "Done";
?>
--EXPECT--
string(5) "UTF-8"
Done
