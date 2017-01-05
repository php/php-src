--TEST--
recode_string() function - Testing string conversions between latin1, UTF-8 and html
--SKIPIF--
<?php if (!extension_loaded("recode")) print "skip"; ?>
--FILE--
<?php
function ascii2hex($ascii) {
    $hex = '';
    for ($i = 0; $i < strlen($ascii); $i++) {
        $byte = dechex(ord($ascii{$i}));
        $byte = str_repeat('0', 2 - strlen($byte)).$byte;
        $hex .= $byte . " ";
    }
    return $hex;
}

function hex2ascii($hex){
    $ascii='';
    $hex=str_replace(" ", "", $hex);
    for($i=0; $i<strlen($hex); $i=$i+2) {
        $ascii .= chr(hexdec(substr($hex, $i, 2)));
    }
    return($ascii);
}

$lat1_hex_org = '31 32 33 e5 e4 f6 61 62 63';
$utf8_hex = ascii2hex(recode_string('lat1..utf-8', hex2ascii($lat1_hex_org)));
$html = recode_string('utf-8..html', hex2ascii($utf8_hex));
$lat1_hex = ascii2hex(recode_string('html..lat1', $html));

echo "#" . $utf8_hex . "#\n";
echo "#" . $html . "#\n";
echo "#" . $lat1_hex . "#\n";
?>
--EXPECT--
#31 32 33 c3 a5 c3 a4 c3 b6 61 62 63 #
#123&aring;&auml;&ouml;abc#
#31 32 33 e5 e4 f6 61 62 63 #