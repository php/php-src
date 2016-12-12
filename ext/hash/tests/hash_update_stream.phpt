--TEST--
int hash_update_stream ( resource $context , resource $handle [, int $length = -1 ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version so lower.');
}
require_once(dirname(__FILE__) . '/skip_mhash.inc'); ?>
?>
--FILE--
<?php
$fp = tmpfile();
fwrite($fp, 'The quick brown fox jumped over the lazy dog.');
rewind($fp);

$ctx = hash_init('md5');
hash_update_stream($ctx, $fp);
echo hash_final($ctx);
?>
--EXPECT--
5c6ffbdd40d9556b73a21e63c3e0e904
