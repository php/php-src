--TEST--
GH-24493: upload progress preserves application-generated session records across serializers
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
$dir = __DIR__ . '/gh24493_upload_progress_session_roundtrip';
@mkdir($dir);
$worker = "$dir/worker.php";
file_put_contents($worker, <<<'CODE'
<?php
$property = getenv('TEST_PROPERTY');
$action = getenv('TEST_ACTION');
if ($action === 'upload') {
    echo 'UPLOAD:', $_FILES['file']['error'], ':',
        isset($_SESSION) ? 'exposed' : 'clean', "\n";
    return;
}
session_start();
if ($action === 'seed') {
    $object = new stdClass;
    $object->{$property} = 'BUSINESS_LITERAL';
    $_SESSION = ['object' => $object, 'alias' => $object, 'sentinel' => 'KEEP_LITERAL'];
    session_write_close();
    echo "SEEDED\n";
    return;
}
$object = $_SESSION['object'] ?? null;
$ok = $object instanceof stdClass
    && ($object->{$property} ?? null) === 'BUSINESS_LITERAL'
    && ($_SESSION['sentinel'] ?? null) === 'KEEP_LITERAL'
    && $object === ($_SESSION['alias'] ?? null);
session_write_close();
echo $ok ? "PRESERVED\n" : "LOST\n";
CODE);
$properties = ['normal', '__PHP_Incomplete_Class_Name',
    '__PHP_Incomplete_Class_Serialized', '__PHP_Incomplete_Enum_Case'];
$boundary = 'gh24493boundary';
$body = "--$boundary\r\nContent-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\nkey\r\n"
    . "--$boundary\r\nContent-Disposition: form-data; name=\"file\"; filename=\"normal.txt\"\r\n\r\nabc\r\n"
    . "--$boundary--\r\n";
$ini = ['session.save_path' => $dir, 'session.save_handler' => 'files',
    'session.use_strict_mode' => '0', 'session.use_cookies' => '1',
    'session.use_only_cookies' => '1', 'session.cache_limiter' => '',
    'session.name' => 'PHPSESSID', 'session.upload_progress.enabled' => '1',
    'session.upload_progress.name' => 'PHP_SESSION_UPLOAD_PROGRESS',
    'session.upload_progress.min_freq' => '0', 'session.upload_progress.freq' => '1',
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
        foreach ($properties as $index => $property) {
            $sid = 'roundtrip' . $cleanup . $index;
            @unlink("$dir/sess_$sid");
            $cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-q'];
            $settings = $ini + ['session.serialize_handler' => $serializer,
                'session.upload_progress.cleanup' => (string) $cleanup];
            foreach ($settings as $name => $value) {
                $cmd[] = '-d';
                $cmd[] = "$name=$value";
            }
            $cmd[] = '-f';
            $cmd[] = $worker;
            $ok = true;
            foreach (['seed', 'upload', 'read'] as $action) {
                $env = getenv();
                $env['REDIRECT_STATUS'] = '1';
                $env['REQUEST_METHOD'] = $action === 'upload' ? 'POST' : 'GET';
                $env['SCRIPT_FILENAME'] = $worker;
                $env['PATH_TRANSLATED'] = $worker;
                $env['QUERY_STRING'] = '';
                $env['HTTP_COOKIE'] = "PHPSESSID=$sid";
                $env['CONTENT_TYPE'] = "multipart/form-data; boundary=$boundary";
                $env['CONTENT_LENGTH'] = $action === 'upload' ? (string) strlen($body) : '0';
                $env['TEST_PROPERTY'] = $property;
                $env['TEST_ACTION'] = $action;
                $proc = proc_open($cmd, [0 => ['pipe','r'], 1 => ['pipe','w'], 2 => ['pipe','w']], $pipes, $dir, $env);
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
                    'seed' => 'SEEDED', 'upload' => 'UPLOAD:0:clean', 'read' => 'PRESERVED'
                };
                $ok = $ok && $exit === 0 && $error === '' && str_contains($output, $expected)
                    && !str_contains($output, 'Warning:');
                if ($action === 'seed') {
                    $record = file_get_contents("$dir/sess_$sid");
                } elseif ($action === 'upload' && $property !== 'normal') {
                    /* A rejected progress update must not rewrite even a partial record. */
                    $ok = $ok && file_exists("$dir/sess_$sid")
                        && file_get_contents("$dir/sess_$sid") === $record;
                }
            }
            printf("%s cleanup=%d property=%d %s\n", $serializer, $cleanup, $index, $ok ? 'OK' : 'FAIL');
        }
    }
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh24493_upload_progress_session_roundtrip';
foreach (glob("$dir/*") as $file) {
    unlink($file);
}
rmdir($dir);
?>
--EXPECT--
php cleanup=0 property=0 OK
php cleanup=0 property=1 OK
php cleanup=0 property=2 OK
php cleanup=0 property=3 OK
php cleanup=1 property=0 OK
php cleanup=1 property=1 OK
php cleanup=1 property=2 OK
php cleanup=1 property=3 OK
php_binary cleanup=0 property=0 OK
php_binary cleanup=0 property=1 OK
php_binary cleanup=0 property=2 OK
php_binary cleanup=0 property=3 OK
php_binary cleanup=1 property=0 OK
php_binary cleanup=1 property=1 OK
php_binary cleanup=1 property=2 OK
php_binary cleanup=1 property=3 OK
php_serialize cleanup=0 property=0 OK
php_serialize cleanup=0 property=1 OK
php_serialize cleanup=0 property=2 OK
php_serialize cleanup=0 property=3 OK
php_serialize cleanup=1 property=0 OK
php_serialize cleanup=1 property=1 OK
php_serialize cleanup=1 property=2 OK
php_serialize cleanup=1 property=3 OK
