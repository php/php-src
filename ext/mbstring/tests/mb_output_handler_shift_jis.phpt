--TEST--
mb_output_handler() (Shift_JIS)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=mb_output_handler
mbstring.internal_encoding=Shift_JIS
mbstring.http_output=EUC-JP
--FILE--
<?php
// Shift_JIS
var_dump("ƒeƒXƒg—p“ú–{Œê•¶Žš—ñB‚±‚Ìƒ‚ƒWƒ…[ƒ‹‚ÍPHP‚Éƒ}ƒ‹ƒ`ƒoƒCƒgŠÖ”‚ð’ñ‹Ÿ‚µ‚Ü‚·B");
?>
--EXPECT--
string(73) "¥Æ¥¹¥ÈÍÑÆüËÜ¸ìÊ¸»úÎó¡£¤³¤Î¥â¥¸¥å¡¼¥ë¤ÏPHP¤Ë¥Þ¥ë¥Á¥Ð¥¤¥È´Ø¿ô¤òÄó¶¡¤·¤Þ¤¹¡£"
