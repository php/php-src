--TEST--
iconv_strlen(), iconv_substr(): strings around and beyond the conversion buffer size
--EXTENSIONS--
iconv
--FILE--
<?php
foreach (["a", "é", "€", "𝄞"] as $char) {
    foreach ([0, 1, 1023, 1024, 1025, 2048, 2049, 1000000] as $count) {
        $string = str_repeat($char, $count);
        if (iconv_strlen($string, "UTF-8") !== $count) {
            echo "iconv_strlen() mismatch for $count x $char\n";
        }
    }
}

$string = str_repeat("é", 1000000);
var_dump(iconv_substr($string, 5, 3, "UTF-8"));
var_dump(iconv_substr($string, -3, 3, "UTF-8"));
var_dump(iconv_strlen("aé€𝄞", "UTF-8"));
var_dump(iconv_strlen(str_repeat("é", 1500) . "\xFF", "UTF-8"));
?>
--EXPECTF--
string(6) "ééé"
string(6) "ééé"
int(4)

Notice: iconv_strlen(): Detected an illegal character in input string in %s on line %d
bool(false)
