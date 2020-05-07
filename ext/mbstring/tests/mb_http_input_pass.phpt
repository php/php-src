--TEST--
mb_http_input() with pass encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
?>
--POST--
a=ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
--GET--
b=ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
--INI--
mbstring.encoding_translation=1
input_encoding=pass
--FILE--
<?php

echo $_POST['a']."\n";
echo $_GET['b']."\n";

// Get encoding
$enc = mb_http_input('P');
var_dump($enc);

?>
--EXPECT--
ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
string(4) "pass"
