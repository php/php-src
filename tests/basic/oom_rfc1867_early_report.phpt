--TEST--
rfc1867: the soft limit is reported immediately after input parsing
--SKIPIF--
<?php require __DIR__ . "/oom_rfc1867_skipif.inc"; ?>
--FILE--
<?php
require __DIR__ . '/oom_rfc1867.inc';

$dir = sys_get_temp_dir();
$scripts = [];
foreach ([1, 60000] as $stmts) {
    $file = $dir . "/oom_rfc1867_early_report_$stmts.php";
    file_put_contents($file, "<?php\n"
        . str_repeat("\$x = 1 + 2;\n", $stmts)
        . "printf(\"post=%d files=%d\\n\", count(\$_POST), count(\$_FILES));\n");
    $scripts[] = $file;
}

foreach ($scripts as $script) {
    oom_rfc1867_request(function ($fh) {
        fwrite($fh, '--' . OOM_RFC1867_BOUNDARY . "\r\n"
            . "Content-Disposition: form-data; name=\"f\"\r\n\r\n");
        oom_rfc1867_fill($fh, 6 * 1024 * 1024, 'x');
        fwrite($fh, "\r\n");
    }, '8M', [], $script);
}

foreach ($scripts as $script) {
    @unlink($script);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in Unknown on line 0
