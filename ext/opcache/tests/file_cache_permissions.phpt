--TEST--
opcache.file_cache_permissions
--EXTENSIONS--
opcache
--SKIPIF--
<?php
$fileCache = sys_get_temp_dir() . '/9664b02e-6f64-42dd-b088-99f3dccc3422';
@mkdir($fileCache, 0777, true);
?>
--INI--
opcache.enable="1"
opcache.enable_cli="1"
opcache.file_cache="{TMP}/9664b02e-6f64-42dd-b088-99f3dccc3422"
opcache.file_cache_only="1"
opcache.file_update_protection="0"
opcache.file_cache_permissions="0660"
--FILE--
<?php
$fileCache = sys_get_temp_dir() . '/9664b02e-6f64-42dd-b088-99f3dccc3422';
$iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($fileCache, FilesystemIterator::SKIP_DOTS));
foreach ($iterator as $splFileInfo) {
    if (!$splFileInfo->isFile()) {
        continue;
    }
    if (str_ends_with($splFileInfo->getPathname(), '.bin')) {
        var_dump(sprintf('%o', fileperms($splFileInfo->getPathname()) & 0o777));
        break;
    }
}
?>
--EXPECT--
string(3) "660"
