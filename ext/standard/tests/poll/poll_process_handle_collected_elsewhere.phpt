--TEST--
Io\Poll\ProcessHandle: a process collected elsewhere or not our child is reported once, in every context
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsProcessHandles()) die("skip no process handle source on this platform");
?>
--FILE--
<?php
use Io\Poll\{Context, Event, ProcessHandle};
$zero = Time\Duration::fromSeconds(0);

// Reaped by pcntl_waitpid() before the context looked
[$r, $w] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$pid = pcntl_fork();
if ($pid == 0) {
    fread($r, 1);
    exit(3);
}
$h = new ProcessHandle($pid);
$c = new Context;
$c->add($h, [Event::Process]);
fwrite($w, "x");
var_dump(pcntl_waitpid($pid, $status) === $pid, pcntl_wexitstatus($status));
$events = $c->wait(Time\Duration::fromSeconds(5));
var_dump(count($events), $events[0]->hasTriggered(Event::Process), $h->getStatus());
var_dump(count($c->wait($zero)));

// Reaped by one context, reported by both, once each
$pid = pcntl_fork();
if ($pid == 0) {
    fread($r, 1);
    exit(4);
}
$h = new ProcessHandle($pid);
$c1 = new Context;
$c2 = new Context;
$c1->add($h, [Event::Process]);
$c2->add($h, [Event::Process]);
fwrite($w, "x");
var_dump(count($c1->wait(Time\Duration::fromSeconds(5))), pcntl_wexitstatus($h->getStatus()));
var_dump(count($c2->wait($zero)), count($c1->wait($zero)), count($c2->wait($zero)));
?>
--EXPECT--
bool(true)
int(3)
int(1)
bool(true)
NULL
int(0)
int(1)
int(4)
int(1)
int(0)
int(0)
