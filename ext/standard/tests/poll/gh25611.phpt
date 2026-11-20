--TEST--
GH-25611 (Io\Poll\Context shares its backend state with a forked child)
--CREDITS--
alies-dev
--EXTENSIONS--
pcntl
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

use Io\Poll\Event;

function wait_names(Io\Poll\Context $context, int $ms): string {
    $names = [];
    foreach ($context->wait(Time\Duration::fromMilliseconds($ms)) as $watcher) {
        $names[] = $watcher->getData();
    }
    return $names ? implode(',', $names) : 'none';
}

echo "child adds a handle", PHP_EOL;
$context = pt_new_stream_poll();
[$reader, $writer] = pt_new_socket_pair();
[$idle, $idleWriter] = pt_new_socket_pair();
$pid = pcntl_fork();
$watcher = pt_stream_poll_add($context, $pid === 0 ? $reader : $idle, [Event::Read], $pid === 0 ? 'child' : 'parent');
if ($pid === 0) {
    fwrite($writer, 'x');
    echo "child: ", wait_names($context, 1000), PHP_EOL;
    exit(0);
}
pcntl_waitpid($pid, $status);
echo "parent: ", wait_names($context, 100), PHP_EOL;
unset($context, $watcher);

echo "child closes a watched stream", PHP_EOL;
$context = pt_new_stream_poll();
[$reader, $writer] = pt_new_socket_pair();
$watcher = pt_stream_poll_add($context, $reader, [Event::Read], 'parent');
$pid = pcntl_fork();
if ($pid === 0) {
    fclose($reader);
    exit(0);
}
pcntl_waitpid($pid, $status);
fwrite($writer, 'x');
echo "parent: ", wait_names($context, 1000), PHP_EOL;
unset($context, $watcher);

echo "child waits on an inherited watcher", PHP_EOL;
$context = pt_new_stream_poll();
[$reader, $writer] = pt_new_socket_pair();
[$other, $otherWriter] = pt_new_socket_pair();
$watcher = pt_stream_poll_add($context, $reader, [Event::Read], 'inherited');
$pid = pcntl_fork();
if ($pid === 0) {
    $watcher->modifyEvents([Event::Read, Event::OneShot]);
    pt_stream_poll_add($context, $other, [Event::Read], 'child only');
    fwrite($otherWriter, 'x');
    echo "child: ", wait_names($context, 1000), PHP_EOL;
    exit(0);
}
pcntl_waitpid($pid, $status);
echo "parent: ", wait_names($context, 100), PHP_EOL;
fwrite($writer, 'x');
echo "parent: ", wait_names($context, 1000), PHP_EOL;
echo "parent: ", wait_names($context, 100), PHP_EOL;
?>
--EXPECT--
child adds a handle
child: child
parent: none
child closes a watched stream
parent: parent
child waits on an inherited watcher
child: child only
parent: none
parent: inherited
parent: inherited
