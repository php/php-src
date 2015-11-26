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

$html_file = fopen(realpath(dirname(__FILE__)) . '/html.raw', 'r');
$utf_8_filepath = realpath(dirname(__FILE__)) . '/utf8.raw';
$utf_8_file = fopen($utf_8_filepath, 'w+');

recode_file('html..utf8', $html_file, $utf_8_file);

rewind($utf_8_file);
echo '#' . ascii2hex(fread($utf_8_file, filesize($utf_8_filepath))) . "#\n";

fclose($html_file);
fclose($utf_8_file);

unlink($utf_8_filepath);
?>
--EXPECT--
#31 32 33 c3 a5 c3 a4 c3 b6 61 62 63 #
