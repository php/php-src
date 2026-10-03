--TEST--
Io\Poll\ProcessHandle: the exit is reported with its status and the child stays waitable, with rusage and by group
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsProcessHandles()) die("skip no process handle source on this platform");
?>
--FILE--
<?php
use Io\Poll\{Context, Event, ProcessHandle};

function observe(int $code): array {
    $pid = pcntl_fork();
    if ($pid == 0) {
        if ($code < 0) {
            posix_kill(posix_getpid(), -$code);
        }
        exit($code);
    }
    $h = new ProcessHandle($pid);
    $c = new Context;
    $c->add($h, [Event::Process]);
    $events = $c->wait(Time\Duration::fromSeconds(5));
    return [$pid, $h, count($events)];
}

echo "-- status recorded, child still waitable\n";
[$a, $h, $n] = observe(3);
var_dump($n, pcntl_wexitstatus($h->getStatus()));
var_dump(pcntl_waitpid($a, $st, WNOHANG) === $a, pcntl_wexitstatus($st));
var_dump(pcntl_waitpid($a, $st));
var_dump(pcntl_wexitstatus($h->getStatus()));

echo "-- killed\n";
[$a, $h] = observe(-SIGKILL);
var_dump(pcntl_wifsignaled($h->getStatus()), pcntl_wtermsig($h->getStatus()) === SIGKILL);
var_dump(pcntl_waitpid($a, $st) === $a, pcntl_wtermsig($st) === SIGKILL);

echo "-- rusage\n";
[$a] = observe(4);
var_dump(pcntl_waitpid($a, $st, 0, $ru) === $a, pcntl_wexitstatus($st), is_array($ru));
[$a] = observe(5);
var_dump(pcntl_wait($st, 0, $ru) === $a, pcntl_wexitstatus($st));

echo "-- by process group\n";
[$a] = observe(6);
var_dump(pcntl_waitpid(0, $st) === $a, pcntl_wexitstatus($st));
[$a] = observe(7);
var_dump(pcntl_waitpid(-posix_getpgrp(), $st) === $a, pcntl_wexitstatus($st));

echo "-- a second context reports it too\n";
[$a, $h] = observe(8);
$c = new Context;
$c->add($h, [Event::Process]);
var_dump(count($c->wait(Time\Duration::fromSeconds(0))));
$h2 = new ProcessHandle($a);
$c->add($h2, [Event::Process]);
var_dump(count($c->wait(Time\Duration::fromSeconds(5))), pcntl_wexitstatus($h2->getStatus()));
var_dump(pcntl_waitpid($a, $st) === $a);

echo "-- collected elsewhere\n";
$pid = pcntl_fork();
if ($pid == 0) {
    exit(9);
}
$h = new ProcessHandle($pid);
pcntl_waitpid($pid, $st);
$c = new Context;
$c->add($h, [Event::Process]);
var_dump(count($c->wait(Time\Duration::fromSeconds(5))), $h->getStatus());
?>
--EXPECT--
-- status recorded, child still waitable
int(1)
int(3)
bool(true)
int(3)
int(-1)
int(3)
-- killed
bool(true)
bool(true)
bool(true)
bool(true)
-- rusage
bool(true)
int(4)
bool(true)
bool(true)
int(5)
-- by process group
bool(true)
int(6)
bool(true)
int(7)
-- a second context reports it too
int(1)
int(1)
int(8)
bool(true)
-- collected elsewhere
int(1)
NULL
