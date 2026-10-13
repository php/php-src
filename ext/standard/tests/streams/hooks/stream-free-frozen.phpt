--TEST--
IO hooks: a stream frozen by a suspended op is not freed under it
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS');

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

/* proc_close() closes the pipes with zend_list_close(): the one being read
 * stays open */
$p = proc_open("exec $php $args -r " . escapeshellarg('fgets(STDIN); echo "bye";'),
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
$out = [];
$scheduler->spawn(function () use ($pipes, &$out) {
    $out['read'] = fread($pipes[1], 10);
    $out['after'] = is_resource($pipes[1]);
});
$scheduler->spawn(function () use ($p, &$out) {
    $out['proc_close'] = proc_close($p);
});
$scheduler->loop();
var_dump($out['read'], $out['after'], $out['proc_close']);
var_dump(fclose($pipes[1]));
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(3) "bye"
bool(true)
int(0)
bool(true)
