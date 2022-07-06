--TEST--
Test imap_binary() function : basic functionality
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
echo "*** Testing imap_binary() : basic functionality ***\n";

echo "Encode as short string\n";
$str = 'This is an example string to be base 64 encoded';
$base64 = imap_binary($str);
var_dump(bin2hex($base64));

echo "Encode a string which results in more than 60 charters of output\n";
$str = 'This is a long string with results in more than 60 characters of output';
$base64 = imap_binary($str);
var_dump(bin2hex($base64));

echo "Encode a string with special characters\n";
$str = '_+-={][];;@~#?/>.<,';
$base64 = imap_binary($str);
var_dump(bin2hex($base64));

echo "Encode some hexadecimal data\n";
$hex = 'x00\x01\x02\x03\x04\x05\x06\xFA\xFB\xFC\xFD\xFE\xFF';
$base64 = imap_binary($hex);
var_dump(bin2hex($base64));

?>
--EXPECT--
*** Testing imap_binary() : basic functionality ***
Encode as short string
string(136) "5647687063794270637942686269426c654746746347786c49484e30636d6c755a794230627942695a53426959584e6c49445930494756755932396b0d0a5a57513d0d0a"
Encode a string which results in more than 60 charters of output
string(200) "56476870637942706379426849477876626d6367633352796157356e4948647064476767636d567a64577830637942706269427462334a6c4948526f0d0a595734674e6a416759326868636d466a64475679637942765a694276645852776458513d0d0a"
Encode a string with special characters
string(60) "5879737450587464573130374f30422b497a3876506934384c413d3d0d0a"
Encode some hexadecimal data
string(144) "65444177584867774d5678344d444a636544417a584867774e4678344d445663654441325848684751567834526b4a6365455a4458486847524678340d0a526b566365455a470d0a"
