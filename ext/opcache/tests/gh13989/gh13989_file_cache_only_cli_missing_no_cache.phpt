--TEST--
GH-13989: CLI should show "Could not open input file" when file_cache_only is not enabled
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$script = __DIR__ . '/gh13989_nonexistent_script.php';

// Without file_cache_only: missing file must produce the original error
$out = shell_exec("$php -n $script 2>&1");
echo "Without opcache: " . trim($out) . "\n";

// With opcache loaded but file_cache_only=0: same original error
$out = shell_exec("$php -n -dopcache.enable=1 -dopcache.enable_cli=1 -dopcache.file_cache_only=0 $script 2>&1");
echo "With opcache, file_cache_only=0: " . trim($out) . "\n";

echo "done\n";
?>
--EXPECTF--
Without opcache: Could not open input file: %sgh13989_nonexistent_script.php
With opcache, file_cache_only=0: Could not open input file: %sgh13989_nonexistent_script.php
done
