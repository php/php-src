--TEST--
Io\Poll: the poll() backend reports a descriptor closed behind its back as an error, once
--EXTENSIONS--
ffi
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die("skip not for Windows");
if (!Io\Poll\Backend::Poll->isAvailable()) die("skip poll backend not available");
if (!is_dir('/proc/self/fd')) die("skip needs /proc/self/fd");
?>
--INI--
ffi.enable=1
--FILE--
<?php
use Io\Poll\{Context, Event, Backend};
$libc = FFI::cdef("int close(int fd);");
function socket_fds(): array {
    $fds = [];
    foreach (scandir('/proc/self/fd') as $f) {
        if (is_numeric($f) && str_starts_with((string) @readlink("/proc/self/fd/$f"), 'socket:')) {
            $fds[] = (int) $f;
        }
    }
    return $fds;
}
$before = socket_fds();
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$new = array_diff(socket_fds(), $before);
$c = new Context(Backend::Poll);
$c->add(new StreamPollHandle($a), [Event::Read], 'a');
$libc->close(min($new));
for ($i = 0; $i < 2; $i++) {
    $events = $c->wait(Time\Duration::fromMilliseconds(100));
    echo count($events);
    foreach ($events as $w) {
        echo " ", $w->getData(), " ", $w->hasTriggered(Event::Error) ? "error" : "no error";
    }
    echo "\n";
}
?>
--EXPECT--
1 a error
0
