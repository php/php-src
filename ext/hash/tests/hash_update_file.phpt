--TEST--
bool hash_update_file ( resource $hcontext , string $filename [, resource $scontext = NULL ] );
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
$ctx = hash_init('md5');
$filePath = __DIR__ . DIRECTORY_SEPARATOR . 'sha1.phpt';
fopen($filePath, "r");
var_dump(hash_update_file($ctx, $filePath));
hash_final($ctx);
?>
--EXPECT--
bool(true)
