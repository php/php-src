--TEST--
Redirection support in proc_open
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');
try {
    proc_open([$php], [['redirect']], $pipes);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    proc_open([$php], [['redirect', 'foo']], $pipes);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    proc_open([$php], [['redirect', 42]], $pipes);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\nWith pipe:\n";
$cmd = [$php, '-r', 'echo "Test\n"; fprintf(STDERR, "Error");'];
$proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
var_dump($pipes);
var_dump(stream_get_contents($pipes[1]));
proc_close($proc);

echo "\nWith filename:\n";
$fileName = __DIR__ . '/proc_open_redirect.txt';
$proc = proc_open($cmd, [1 => ['file', $fileName, 'w'], 2 => ['redirect', 1]], $pipes);
var_dump($pipes);
proc_close($proc);
var_dump(file_get_contents($fileName));
unlink($fileName);

echo "\nWith file:\n";
$file = fopen($fileName, 'w');
$proc = proc_open($cmd, [1 => $file, 2 => ['redirect', 1]], $pipes);
var_dump($pipes);
proc_close($proc);
fclose($file);
var_dump(file_get_contents($fileName));
unlink($fileName);

echo "\nWith inherited stdout:\n";
$proc = proc_open($cmd, [2 => ['redirect', 1]], $pipes);
proc_close($proc);

?>
--EXPECTF--
Missing redirection target
Redirection target must be of type int, string given

Warning: proc_open(): Redirection target 42 not found in %s

With pipe:
array(1) {
  [1]=>
  resource(4) of type (stream)
}
string(10) "Test
Error"

With filename:
array(0) {
}
string(10) "Test
Error"

With file:
array(0) {
}
string(10) "Test
Error"

With inherited stdout:
Test
Error
