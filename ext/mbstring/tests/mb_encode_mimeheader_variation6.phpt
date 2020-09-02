--TEST--
Test mb_encode_mimeheader() function : usage variations - Pass different strings to $linefeed arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_encode_mimeheader') or die("skip mb_encode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/* (string $str [, string $charset [, string $transfer_encoding [, string $linefeed [, int $indent]]]])
 * Description: Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?=
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass different strings to $linefeed argument
 */

echo "*** Testing mb_encode_mimeheader() : usage variations ***\n";

mb_internal_encoding('utf-8');

$linefeeds = array("\r\n",
                   "\n",
                   "---");
$str = base64_decode('zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868zrXOvc6/LiAwMTIzNDU2Nzg5Lg==');

$iterator = 1;
foreach ($linefeeds as $linefeed) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(mb_encode_mimeheader($str, 'utf-8', 'B', $linefeed));
    $iterator++;
}


echo "Done";
?>
--EXPECT--
*** Testing mb_encode_mimeheader() : usage variations ***

-- Iteration 1 --
string(115) "=?UTF-8?B?zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868?=
 =?UTF-8?B?zrXOvc6/LiAwMTIzNDU2Nzg5Lg==?="

-- Iteration 2 --
string(114) "=?UTF-8?B?zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868?=
 =?UTF-8?B?zrXOvc6/LiAwMTIzNDU2Nzg5Lg==?="

-- Iteration 3 --
string(116) "=?UTF-8?B?zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868?=--- =?UTF-8?B?zrXOvc6/LiAwMTIzNDU2Nzg5Lg==?="
Done
