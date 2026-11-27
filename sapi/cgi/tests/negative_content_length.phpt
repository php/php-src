--TEST--
php-cgi clamps negative CONTENT_LENGTH to 0 without aborting or underflowing read length
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
include "include.inc";

$cgi = get_cgi_path();
reset_env_vars();

$tmp = __DIR__ . '/negative_content_length_' . getmypid() . '.php';
file_put_contents($tmp, '<?php echo "OK:" . strlen(file_get_contents("php://input")) . ":" . json_encode($_POST) . "\n"; ?>');

$env = [
    'REDIRECT_STATUS' => '200',
    'REQUEST_METHOD' => 'POST',
    'SCRIPT_FILENAME' => $tmp,
    'CONTENT_TYPE' => 'application/x-www-form-urlencoded',
    'CONTENT_LENGTH' => '-1',
    'GATEWAY_INTERFACE' => 'CGI/1.1',
];

$spec = [
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];

$proc = proc_open([$cgi, '-n'], $spec, $pipes, null, $env);
fwrite($pipes[0], 'a=1');
fclose($pipes[0]);
$stdout = stream_get_contents($pipes[1]);
$stderr = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
$code = proc_close($proc);

@unlink($tmp);

echo "EXIT=" . $code . "\n";
echo "BODY=" . (preg_match('/OK:\d+:[^\r\n]+/', $stdout, $m) ? $m[0] : 'MISSING') . "\n";
echo "STDERR_EMPTY=" . ($stderr === '' ? 'YES' : 'NO:' . trim($stderr)) . "\n";
?>
--EXPECT--
EXIT=0
BODY=OK:0:[]
STDERR_EMPTY=YES
