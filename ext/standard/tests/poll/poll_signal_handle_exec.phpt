--TEST--
Io\Poll\SignalHandle: programs started while a signal is watched do not inherit the block
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
if (PHP_OS_FAMILY === 'Windows') die("skip POSIX signal masks");
?>
--FILE--
<?php
$ctx = new Io\Poll\Context();
$handle = new Io\Poll\SignalHandle([SIGTERM]);
$watcher = $ctx->add($handle, [Io\Poll\Event::Signal]);

// A PHP child reports whether it runs with SIGTERM blocked
$blocked = function (string $out): bool {
    return trim($out) === '1';
};
$cmd = escapeshellarg(PHP_BINARY) . ' -r '
    . escapeshellarg('pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2], $mask); echo (int) in_array(SIGTERM, $mask);');

var_dump($blocked(shell_exec($cmd)));
exec($cmd, $lines);
var_dump($blocked(implode("\n", $lines)));
ob_start();
system($cmd);
var_dump($blocked(ob_get_clean()));
ob_start();
passthru($cmd);
var_dump($blocked(ob_get_clean()));
var_dump($blocked(stream_get_contents($p = popen($cmd, 'r'))));
pclose($p);
$proc = proc_open($cmd, [1 => ['pipe', 'w']], $pipes);
var_dump($blocked(stream_get_contents($pipes[1])));
proc_close($proc);

// The parent keeps it blocked
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2], $mask);
var_dump(in_array(SIGTERM, $mask));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
