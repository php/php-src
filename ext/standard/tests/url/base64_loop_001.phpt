--TEST--
Test base64_encode() and base64_decode() function : loop mode
--FILE--
<?php
/*
 * Test base64_encode/base64_decode with loop mode.
 */

echo "*** Testing base64_encode() and base64_decode(): loop mode ***\n";

$result = "";
$expect_result = "";
$str = "";

for ($i=0; $i<1024; $i++) {
    $c = pack("c", $i);
    $str .= $c;
    $str_enc = base64_encode($str);
    $str_dec = base64_decode($str_enc);

    $result .= strcmp($str, $str_dec);
    $expect_result .= '0';
}
if ($result != $expect_result) {
    echo "TEST FAILED\n";
} else {
    echo "TEST PASSED\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing base64_encode() and base64_decode(): loop mode ***
TEST PASSED
Done
