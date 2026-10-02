--TEST--
dl() of a frameless function does not keep the freed function record
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die('skip setarch -R is required so dl() remaps the extension at the stale handler address');
}
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die('skip php-cgi not available');
}
$setarch = trim((string) shell_exec('command -v setarch'));
if ($setarch === '') {
    die('skip setarch -R is required so dl() remaps the extension at the stale handler address');
}
$arch = php_uname('m');
exec($setarch . ' ' . escapeshellarg($arch) . ' -R true', $setarch_out, $setarch_code);
if ($setarch_code !== 0) {
    die('skip setarch -R cannot run on ' . $arch);
}
$so = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'dl_test.so';
if (!file_exists($so)) {
    die('skip dl_test extension is not built (tried ' . $so . ')');
}
?>
--FILE--
<?php
$cmd = 'env -u SCRIPT_FILENAME -u PATH_TRANSLATED -u REDIRECT_STATUS -u REQUEST_METHOD -u QUERY_STRING'
    . ' USE_ZEND_ALLOC=0 ASAN_OPTIONS=' . escapeshellarg('detect_leaks=0:halt_on_error=1:abort_on_error=1')
    . ' setarch ' . escapeshellarg(php_uname('m')) . ' -R '
    . escapeshellarg(getenv('TEST_PHP_CGI_EXECUTABLE'))
    . ' -n -q -T 2 -d enable_dl=1 -d extension_dir=' . escapeshellarg(ini_get('extension_dir'))
    . ' -d cgi.security_limit_extensions=.inc'
    . ' ' . escapeshellarg(__DIR__ . '/frameless_temporary_cgi.inc');
$proc = proc_open($cmd, [
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
], $pipes);
fclose($pipes[0]);
$out = stream_get_contents($pipes[1]);
stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
echo $out;
$code = proc_close($proc);
if ($code !== 0) {
    echo "exit:$code\n";
}
?>
--EXPECT--
int(7)
dl_test_frameless(): Argument #1 ($value) must be of type int, string given
int(7)
dl_test_frameless(): Argument #1 ($value) must be of type int, string given
