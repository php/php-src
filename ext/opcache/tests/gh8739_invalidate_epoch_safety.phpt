--TEST--
GH-8739: opcache_invalidate() does not crash when reader holds cached pointer
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.revalidate_freq=0
opcache.validate_timestamps=1
--FILE--
<?php
$status = opcache_get_status(false);
if (!$status || !$status['opcache_enabled']) {
    die("skip: OPcache not enabled");
}

$tmpFile = tempnam(sys_get_temp_dir(), 'opcache_inv_');
$tmpPhp = $tmpFile . '.php';
rename($tmpFile, $tmpPhp);
file_put_contents($tmpPhp, '<?php function opcache_inv_fn() { return "before"; }');

include $tmpPhp;
echo opcache_inv_fn() . "\n";

/* Invalidate while this request still holds pointers to the cached
 * op_array. Under the epoch-based reclamation model, invalidate()
 * must not corrupt memory the current request is reading. */
var_dump(opcache_invalidate($tmpPhp, true));

/* The function reference already resolved in this request continues
 * to work because its op_array is kept alive by our published epoch. */
echo opcache_inv_fn() . "\n";

unlink($tmpPhp);
echo "OK\n";
?>
--EXPECT--
before
bool(true)
before
OK
