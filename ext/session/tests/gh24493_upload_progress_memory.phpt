--TEST--
GH-24493: aborted upload progress updates release the lazy-write buffer
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die('skip CGI not available');
}
if (!function_exists('proc_open')) {
    die('skip proc_open not available');
}
?>
--FILE--
<?php
$dir = __DIR__ . '/gh24493_upload_progress_memory';
@mkdir($dir);
$worker = "$dir/worker.php";
file_put_contents($worker, <<<'CODE'
<?php
$action = getenv('TEST_ACTION');
if ($action === 'upload') {
    echo 'UPLOAD:', $_FILES['file']['error'], ':',
        file_get_contents($_FILES['file']['tmp_name']), ':',
        isset($_SESSION) ? 'exposed' : 'clean', "\n";
    return;
}
session_start();
if ($action === 'seed') {
    $object = new stdClass;
    $object->__PHP_Incomplete_Class_Name = 'application value';
    /* The restricted decoder rejects the object before reading the payload. */
    $_SESSION = ['object' => $object, 'payload' => str_repeat('A', 5 * 1024 * 1024)];
    session_write_close();
    echo "SEEDED\n";
    return;
}
$ok = ($_SESSION['object']->__PHP_Incomplete_Class_Name ?? null) === 'application value'
    && ($_SESSION['payload'] ?? null) === str_repeat('A', 5 * 1024 * 1024);
session_write_close();
echo $ok ? "PRESERVED\n" : "LOST\n";
CODE);
$boundary = 'sessionmemoryboundary';
$body = "--$boundary\r\nContent-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\nkey\r\n"
    . "--$boundary\r\nContent-Disposition: form-data; name=\"file\"; filename=\"file.txt\"\r\n\r\nabc\r\n"
    . "--$boundary--\r\n";
$ini = ['session.save_path' => $dir, 'session.save_handler' => 'files',
    'session.gc_probability' => '0', 'session.use_strict_mode' => '0',
    'session.use_cookies' => '1', 'session.use_only_cookies' => '1',
    'session.cache_limiter' => '', 'session.name' => 'PHPSESSID',
    'session.lazy_write' => '1', 'session.upload_progress.enabled' => '1',
    'session.upload_progress.name' => 'PHP_SESSION_UPLOAD_PROGRESS',
    'session.upload_progress.freq' => '1', 'session.upload_progress.min_freq' => '0',
    'file_uploads' => '1', 'display_errors' => '1', 'html_errors' => '0'];
$extensionDir = ini_get('extension_dir');
foreach (['session.' . PHP_SHLIB_SUFFIX, 'php_session.' . PHP_SHLIB_SUFFIX] as $module) {
    if ($extensionDir !== false && file_exists("$extensionDir/$module")) {
        $ini['extension_dir'] = $extensionDir;
        $ini['extension'] = $module;
        break;
    }
}
foreach (['php', 'php_binary', 'php_serialize'] as $serializer) {
    foreach ([0, 1] as $cleanup) {
        $sid = 'memory' . $cleanup;
        @unlink("$dir/sess_$sid");
        $ok = true;
        foreach (['seed', 'upload', 'read'] as $action) {
            $cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-q'];
            $settings = $ini + ['session.serialize_handler' => $serializer,
                'session.upload_progress.cleanup' => (string) $cleanup,
                'memory_limit' => $action === 'upload' ? '8M' : '64M'];
            foreach ($settings as $name => $value) {
                $cmd[] = '-d';
                $cmd[] = "$name=$value";
            }
            $cmd[] = '-f';
            $cmd[] = $worker;
            $env = getenv();
            $env['REDIRECT_STATUS'] = '1';
            $env['REQUEST_METHOD'] = $action === 'upload' ? 'POST' : 'GET';
            $env['SCRIPT_FILENAME'] = $worker;
            $env['PATH_TRANSLATED'] = $worker;
            $env['QUERY_STRING'] = '';
            $env['HTTP_COOKIE'] = "PHPSESSID=$sid";
            $env['CONTENT_TYPE'] = "multipart/form-data; boundary=$boundary";
            $env['CONTENT_LENGTH'] = $action === 'upload' ? (string) strlen($body) : '0';
            $env['TEST_ACTION'] = $action;
            $proc = proc_open($cmd, [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes, $dir, $env);
            if ($action === 'upload') {
                fwrite($pipes[0], $body);
            }
            fclose($pipes[0]);
            $output = stream_get_contents($pipes[1]);
            $error = stream_get_contents($pipes[2]);
            fclose($pipes[1]);
            fclose($pipes[2]);
            $exit = proc_close($proc);
            $expected = match ($action) {
                'seed' => 'SEEDED', 'upload' => 'UPLOAD:0:abc:clean', 'read' => 'PRESERVED'
            };
            $ok = $ok && $exit === 0 && $error === '' && str_contains($output, $expected)
                && !str_contains($output, 'Warning:') && !str_contains($output, 'Fatal error:');
            if ($action === 'seed') {
                $recordHash = hash_file('sha256', "$dir/sess_$sid");
            } elseif ($action === 'upload') {
                $ok = $ok && file_exists("$dir/sess_$sid")
                    && hash_file('sha256', "$dir/sess_$sid") === $recordHash;
            }
        }
        printf("%s cleanup=%d %s\n", $serializer, $cleanup, $ok ? 'OK' : 'FAIL');
    }
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh24493_upload_progress_memory';
foreach (glob("$dir/*") as $file) {
    unlink($file);
}
rmdir($dir);
?>
--EXPECT--
php cleanup=0 OK
php cleanup=1 OK
php_binary cleanup=0 OK
php_binary cleanup=1 OK
php_serialize cleanup=0 OK
php_serialize cleanup=1 OK
