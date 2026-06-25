--TEST--
GH-8739: opcache_reset() with epoch-based reclamation does not corrupt cached data
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.revalidate_freq=0
--FILE--
<?php
$status = opcache_get_status(false);
if (!$status || !$status['opcache_enabled']) {
    die("skip: OPcache not enabled");
}

$tmpFile = tempnam(sys_get_temp_dir(), 'opcache_ebr_');
$tmpPhp = $tmpFile . '.php';
rename($tmpFile, $tmpPhp);
file_put_contents($tmpPhp, '<?php class OpcacheEbrTest { const VALUE = 42; }');

include $tmpPhp;
echo "Before reset: " . OpcacheEbrTest::VALUE . "\n";

var_dump(opcache_reset());

/* After reset, the class is still loaded in this request — the actual
 * SHM cleanup is deferred to the next request boundary. The reader's
 * pointer must remain valid for the duration of this request. */
echo "After reset: " . OpcacheEbrTest::VALUE . "\n";

unlink($tmpPhp);
echo "OK\n";
?>
--EXPECT--
Before reset: 42
bool(true)
After reset: 42
OK
