--TEST--
Test md5() function : basic functionality - with raw output
--FILE--
<?php
echo "*** Testing md5() : basic functionality - with raw output***\n";
$str = "Hello World";
$md5_raw = md5($str, true);
var_dump(bin2hex($md5_raw));

$md5 = md5($str, false);

if (strcmp(bin2hex($md5_raw), $md5) == 0 ) {
    echo "TEST PASSED\n";
} else {
    echo "TEST FAILED\n";
    var_dump($md5_raw, $md5);
}

?>
--EXPECT--
*** Testing md5() : basic functionality - with raw output***
string(32) "b10a8db164e0754105b7a99be72e3fe5"
TEST PASSED
