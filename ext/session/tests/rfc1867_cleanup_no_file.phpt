--TEST--
session rfc1867: cleanup does not mint a session or send Set-Cookie when no file is uploaded
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die('skip CGI not available');
}
?>
--FILE--
<?php

$dir = __DIR__ . '/rfc1867_cleanup_no_file';
@mkdir($dir);
$worker = "$dir/worker.php";

file_put_contents($worker, <<<'PHP'
<?php
echo "OK\n";
PHP);

$boundary = '---------------------------20896060251896012921717172737';
$body = "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\nkey\r\n"
      . "--$boundary--\r\n";

$ini = [
    'file_uploads' => '1',
    'session.save_path' => $dir,
    'session.name' => 'PHPSESSID',
    'session.use_strict_mode' => '0',
    'session.use_cookies' => '1',
    'session.use_only_cookies' => '1',
    'session.save_handler' => 'files',
    'session.upload_progress.enabled' => '1',
    'session.upload_progress.cleanup' => '1',
    'session.upload_progress.prefix' => 'upload_progress_',
    'session.upload_progress.name' => 'PHP_SESSION_UPLOAD_PROGRESS',
];

$extension_dir = ini_get('extension_dir');
foreach (['session.' . PHP_SHLIB_SUFFIX, 'php_session.' . PHP_SHLIB_SUFFIX] as $module) {
    if ($extension_dir !== false && file_exists("$extension_dir/$module")) {
        $ini['extension_dir'] = $extension_dir;
        $ini['extension'] = $module;
        break;
    }
}

$cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n'];
foreach ($ini as $name => $value) {
    $cmd[] = '-d';
    $cmd[] = "$name=$value";
}
$cmd[] = '-f';
$cmd[] = $worker;

$env = getenv();
$env['REDIRECT_STATUS'] = '200';
$env['REQUEST_METHOD'] = 'POST';
$env['CONTENT_TYPE'] = "multipart/form-data; boundary=$boundary";
$env['CONTENT_LENGTH'] = (string) strlen($body);
$env['HTTP_COOKIE'] = 'PHPSESSID=rfc1867nofiletest';
$env['SCRIPT_FILENAME'] = $worker;
$env['PATH_TRANSLATED'] = $worker;
$env['QUERY_STRING'] = '';

$pipes = [];
$proc = proc_open(
    $cmd,
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes,
    $dir,
    $env
);
fwrite($pipes[0], $body);
fclose($pipes[0]);
$out = stream_get_contents($pipes[1]);
$err = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
$code = proc_close($proc);

echo "EXIT=$code\n";
echo "HAS_SET_COOKIE=" . (stripos($out, 'Set-Cookie:') !== false ? 'YES' : 'NO') . "\n";
echo "SESS_FILES=" . count(glob("$dir/sess_*")) . "\n";
echo "BODY_OK=" . (str_contains($out, "OK\n") ? 'YES' : 'NO') . "\n";
echo "STDERR_EMPTY=" . ($err === '' ? 'YES' : 'NO:' . trim($err)) . "\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/rfc1867_cleanup_no_file';
@unlink("$dir/worker.php");
foreach (glob("$dir/sess_*") as $f) {
    unlink($f);
}
@rmdir($dir);
?>
--EXPECT--
EXIT=0
HAS_SET_COOKIE=NO
SESS_FILES=0
BODY_OK=YES
STDERR_EMPTY=YES
