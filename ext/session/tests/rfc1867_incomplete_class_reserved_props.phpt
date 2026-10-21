--TEST--
session rfc1867: the upload progress update rejects session data whose objects carry the incomplete class bookkeeping property names
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

$dir = __DIR__ . '/rfc1867_incomplete_class_reserved_props';
@mkdir($dir);
$sid = 'rfc1867reservedprops';
$file = "$dir/sess_$sid";
$worker = "$dir/worker.php";
file_put_contents($worker, '<?php');

/* The handler decodes the record with every object kept as
 * __PHP_Incomplete_Class, which stores the class name of each object and the
 * payload of C:/E: entries under these three property names. Data that already
 * carries one of them cannot be told apart from that bookkeeping, so the decode
 * has to fail rather than let the two meanings be confused. */
$cases = [
    'control'          => 'x|O:8:"stdClass":1:{s:1:"a";i:1;}',
    'Class_Name'       => 'x|O:8:"stdClass":1:{s:27:"__PHP_Incomplete_Class_Name";s:3:"foo";}',
    'Class_Serialized' => 'x|O:8:"stdClass":1:{s:33:"__PHP_Incomplete_Class_Serialized";s:3:"foo";}',
    'Enum_Case'        => 'x|O:8:"stdClass":1:{s:26:"__PHP_Incomplete_Enum_Case";s:3:"foo";}',
    'nested'           => 'x|a:1:{i:0;O:8:"stdClass":1:{s:27:"__PHP_Incomplete_Class_Name";s:3:"foo";}}',
    'mangled'          => 'x|O:8:"stdClass":1:{s:30:"' . "\0" . 'A' . "\0" . '__PHP_Incomplete_Class_Name";s:3:"foo";}',
    'corrupt'          => 'x|not-serialized-at-all',
];

$boundary = '---------------------------20896060251896012921717172737';
$body = "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"PHPSESSID\"\r\n\r\n$sid\r\n"
      . "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\nkey\r\n"
      . "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"file1\"; filename=\"file1.txt\"\r\n\r\n1\r\n"
      . "--$boundary--\r\n";

$ini = [
    'error_reporting' => 'E_ALL',
    'html_errors' => '0',
    'file_uploads' => '1',
    'upload_max_filesize' => '1024',
    'session.save_path' => $dir,
    'session.name' => 'PHPSESSID',
    'session.use_strict_mode' => '0',
    'session.use_cookies' => '0',
    'session.use_only_cookies' => '0',
    'session.cache_limiter' => '',
    'session.save_handler' => 'files',
    'session.serialize_handler' => 'php',
    'session.upload_progress.enabled' => '1',
    'session.upload_progress.cleanup' => '0',
    'session.upload_progress.prefix' => 'upload_progress_',
    'session.upload_progress.name' => 'PHP_SESSION_UPLOAD_PROGRESS',
    'session.upload_progress.freq' => '1%',
    'session.upload_progress.min_freq' => '0.000000001',
];

/* php-cgi is started with -n, so a shared session module has to be named again. */
$extension_dir = ini_get('extension_dir');
foreach (['session.' . PHP_SHLIB_SUFFIX, 'php_session.' . PHP_SHLIB_SUFFIX] as $module) {
    if ($extension_dir !== false && file_exists("$extension_dir/$module")) {
        $ini['extension_dir'] = $extension_dir;
        $ini['extension'] = $module;
        break;
    }
}

$cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-q'];
foreach ($ini as $name => $value) {
    $cmd[] = '-d';
    $cmd[] = "$name=$value";
}
$cmd[] = '-f';
$cmd[] = $worker;

$env = getenv();
$env['REDIRECT_STATUS'] = '1';
$env['REQUEST_METHOD'] = 'POST';
$env['CONTENT_TYPE'] = "multipart/form-data; boundary=$boundary";
$env['CONTENT_LENGTH'] = (string) strlen($body);
$env['SCRIPT_FILENAME'] = $worker;
$env['PATH_TRANSLATED'] = $worker;
$env['QUERY_STRING'] = '';

foreach ($cases as $label => $record) {
    file_put_contents($file, $record);

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
    $out = stream_get_contents($pipes[1]) . stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($proc);

    /* A refused decode must leave exactly one warning: the handler stops there
     * instead of writing the progress into the empty session it was left with,
     * which would add a second warning about the save path. */
    $kept = file_exists($file) && str_starts_with(file_get_contents($file), $record);
    printf("%-16s warnings: %d, decode refused: %-5s record kept: %s\n", $label,
        substr_count($out, 'Warning:'),
        var_export(str_contains($out, 'Session has been destroyed'), true),
        var_export($kept, true));
}

?>
--CLEAN--
<?php
$dir = __DIR__ . '/rfc1867_incomplete_class_reserved_props';
@unlink("$dir/worker.php");
foreach (glob("$dir/sess_*") as $f) {
    unlink($f);
}
@rmdir($dir);
?>
--EXPECT--
control          warnings: 0, decode refused: false record kept: true
Class_Name       warnings: 1, decode refused: true  record kept: false
Class_Serialized warnings: 1, decode refused: true  record kept: false
Enum_Case        warnings: 1, decode refused: true  record kept: false
nested           warnings: 1, decode refused: true  record kept: false
mangled          warnings: 0, decode refused: false record kept: true
corrupt          warnings: 1, decode refused: true  record kept: false
