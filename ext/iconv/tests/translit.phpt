--TEST--
ICONV: LIBICONV transliteration (bug #16069)
--SKIPIF--
<?php include( 'skipif.inc' ); if( iconv('CP932', 'EUC-JP//TRANSLIT', "\x87\x6d") == '' ) die("skip\n"); ?>
--POST--
--GET--
--FILE--
<?php include('translit.inc'); ?>
--EXPECT--
ミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバールミリバール(°Д°)
