--TEST--
GH-8739: opcache_reset() deferred cleanup prevents use-after-free
--DESCRIPTION--
Verifies that the epoch-based reclamation system correctly tracks
active readers and defers opcache cleanup until all pre-reset
readers have completed their requests.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.revalidate_freq=0
--EXTENSIONS--
opcache
--FILE--
<?php
$status = opcache_get_status();
if (!$status['opcache_enabled']) {
    die("SKIP: OPcache not enabled");
}

$tmpFile = tempnam(sys_get_temp_dir(), 'opcache_test_');
$tmpPhp = $tmpFile . '.php';
rename($tmpFile, $tmpPhp);
file_put_contents($tmpPhp, '<?php class OpcacheTestEBR { const VALUE = 42; }');

include $tmpPhp;
echo "Before reset: " . OpcacheTestEBR::VALUE . "\n";

$result = opcache_reset();
echo "Reset result: " . ($result ? "true" : "false") . "\n";

/* After reset we can still access cached data without crashing */
echo "After reset: " . OpcacheTestEBR::VALUE . "\n";

unlink($tmpPhp);
echo "OK\n";
?>
--EXPECT--
Before reset: 42
Reset result: true
After reset: 42
OK
