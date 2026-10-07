--TEST--
test_scheduler: code from -r runs in the main coroutine as a script file does; -B, -R and -E share one main coroutine
--EXTENSIONS--
test_scheduler
--FILE--
<?php
/* The child code avoids the characters escapeshellarg() drops on Windows (" % !). */
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED') . ' ' . getenv('TEST_PHP_EXTRA_ARGS') . ' -d test_scheduler.enable=1';

/* One line each: cmd.exe ends a command at a newline. */
$main = "use function TestScheduler\\spawn; spawn(function () { echo 'spawned', PHP_EOL; }); echo 'main', PHP_EOL;";
$throws = "use function TestScheduler\\spawn; spawn(function () { echo 'spawned', PHP_EOL; }); throw new Exception('from main');";
$exits = "use function TestScheduler\\spawn; spawn(function () { echo 'spawned', PHP_EOL; }); exit(3);";

$file = __DIR__ . '/091_command_line_code.inc';

foreach (['main' => $main, 'throws' => $throws, 'exits' => $exits] as $name => $code) {
    file_put_contents($file, "<?php " . $code);
    exec("$php -f " . escapeshellarg($file) . ' 2>&1', $from_file, $file_status);
    exec("$php -r " . escapeshellarg($code) . ' 2>&1', $from_r, $r_status);
    $from_file = str_replace($file, 'Command line code', implode("\n", $from_file));
    $from_r = implode("\n", $from_r);
    echo "$name: -r ", $from_r === $from_file && $r_status === $file_status ? "as a file" : "differs:\n$from_r\n---\n$from_file", ", status $r_status\n";
    unset($from_file, $from_r);
}

$begin = "TestScheduler\\spawn(function () { echo 'begin spawned', PHP_EOL; });";
$run = "echo 'line ', \$argn, PHP_EOL; TestScheduler\\spawn(function () use (\$argn) { echo 'run spawned: ', \$argn, PHP_EOL; });";
$end = "echo 'end', PHP_EOL; TestScheduler\\spawn(function () { echo 'end spawned', PHP_EOL; });";

$process = proc_open(
    "$php -B " . escapeshellarg($begin) . ' -R ' . escapeshellarg($run) . ' -E ' . escapeshellarg($end) . ' 2>&1',
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w']],
    $pipes
);
fwrite($pipes[0], "a\nb\n");
fclose($pipes[0]);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
echo "stdin status ", proc_close($process), "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/091_command_line_code.inc');
?>
--EXPECT--
main: -r as a file, status 0
throws: -r as a file, status 255
exits: -r as a file, status 3
line a
line b
end
begin spawned
run spawned: a
run spawned: b
end spawned
stdin status 0
