--TEST--
Io\Poll\SignalHandle: the signals it blocks while watched stay unblocked in exec'd children, mail() and pcntl_exec() included
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!is_readable('/proc/self/status')) die("skip needs /proc/self/status");
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--INI--
sendmail_path="grep SigBlk /proc/self/status > {PWD}/poll_signal_handle_child_mask.out; cat > /dev/null"
--FILE--
<?php
function usr1_blocked(string $status_line): string {
    $mask = hexdec(trim(substr($status_line, strpos($status_line, ':') + 1)));
    return ($mask & (1 << (SIGUSR1 - 1))) ? "blocked" : "not blocked";
}

$h = new Io\Poll\SignalHandle([SIGUSR1]);
$ctx = new Io\Poll\Context();
$ctx->add($h, [Io\Poll\Event::Signal]);
echo "self: ", usr1_blocked(preg_replace('/.*^(SigBlk:[^\n]*).*/ms', '$1', file_get_contents('/proc/self/status'))), "\n";

$out = __DIR__ . '/poll_signal_handle_child_mask.out';
var_dump(mail('nobody@example.com', 'subject', 'body'));
echo "mail: ", usr1_blocked(file_get_contents($out)), "\n";

$pid = pcntl_fork();
if ($pid == 0) {
    pcntl_exec('/bin/sh', ['-c', "grep SigBlk /proc/self/status > $out"]);
    exit(1);
}
pcntl_waitpid($pid, $status);
echo "pcntl_exec: ", usr1_blocked(file_get_contents($out)), "\n";

// A failed exec leaves the mask as it was
$pid = pcntl_fork();
if ($pid == 0) {
    @pcntl_exec('/nonexistent/program');
    echo "failed exec: ", usr1_blocked(preg_replace('/.*^(SigBlk:[^\n]*).*/ms', '$1', file_get_contents('/proc/self/status'))), "\n";
    exit(0);
}
pcntl_waitpid($pid, $status);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/poll_signal_handle_child_mask.out');
?>
--EXPECT--
self: blocked
bool(true)
mail: not blocked
pcntl_exec: not blocked
failed exec: blocked
