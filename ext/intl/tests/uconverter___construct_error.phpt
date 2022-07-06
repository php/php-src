--TEST--
Basic UConverter::convert() usage
--INI--
intl.error_level = E_WARNING
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$c = new UConverter('utf-8', "\x80");
var_dump($c);
?>
--EXPECTF--
Warning: UConverter::__construct(): ucnv_open() returned error 4: U_FILE_ACCESS_ERROR in %s on line %d
object(UConverter)#%d (0) {
}
