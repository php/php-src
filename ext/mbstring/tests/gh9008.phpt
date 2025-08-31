--TEST--
GH-9008 (mb_detect_encoding(): wrong results with null $encodings)
--EXTENSIONS--
mbstring
--FILE--
<?php
$string = "<?php

function test()
{

}
";

mb_detect_order(["ASCII", "UUENCODE"]);

var_dump(
    mb_detect_encoding($string, null, true),
    mb_detect_encoding($string, mb_detect_order(), true),
);
?>
--EXPECT--
string(5) "ASCII"
string(5) "ASCII"
