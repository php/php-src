--TEST--
Using proc_open() with invalid cwd
--FILE--
<?php

function wait_for_proc($process) {
    for ($i = 0; $i < 10; $i++) {
        $status = proc_get_status($process);
        if (!$status['running']) {
            return;
        }
        usleep(100000);
    }
    throw new Exception('Process did not terminate');
}

function test($desired_cwd) {
    $php = getenv('TEST_PHP_EXECUTABLE');
    $ds = [STDIN, STDOUT, STDERR];
    $current_cwd = getcwd();
    wait_for_proc(proc_open([$php, '-r', "var_dump(getcwd() != '$current_cwd');"], $ds, $pipes, $desired_cwd));
}

$dir = __DIR__ . '/proc_open_invalid_cwd_no_prot';
mkdir($dir);
chmod($dir, 0666);

test('.');
test('/tmp');
test('/does/not/exist');
test($dir);

?>
--CLEAN--
<?php
$dir = __DIR__ . '/proc_open_invalid_cwd_no_prot';
@rmdir($dir);
?>
--EXPECTF--
bool(false)
bool(true)

Deprecated: proc_open(): Provided cwd does not exist, falling back to current cwd in %s on line %d
bool(false)

Deprecated: proc_open(): Provided cwd does not exist, falling back to current cwd in %s on line %d
bool(false)
