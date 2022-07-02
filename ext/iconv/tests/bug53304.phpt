--TEST--
Bug #53304 (quot_print_decode does not handle lower-case hex digits)
--EXTENSIONS--
iconv
--FILE--
<?php
echo iconv_mime_decode('=?utf-8?Q?Nachricht_=c3=bcber_Kontaktformular_www.inexio.net?=', 0, 'UTF-8') . "\n";
echo iconv_mime_decode('=?utf-8?Q?Nachricht_=C3=BCber_Kontaktformular_www.inexio.net?=', 0, 'UTF-8') . "\n";

?>
--EXPECT--
Nachricht über Kontaktformular www.inexio.net
Nachricht über Kontaktformular www.inexio.net
