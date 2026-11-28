--TEST--
stream_select() with a zero timeout does not sleep when no pipe is ready
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip this test is for Windows platforms only');
?>
--FILE--
<?php
$process = proc_open(
    [getenv('TEST_PHP_EXECUTABLE'), '-n', '-r', 'fgets(STDIN); echo "x"; fgets(STDIN);'],
    [['pipe', 'r'], ['pipe', 'w'], ['file', 'NUL', 'w']],
    $pipes
);

function select($pipe, $seconds, $microseconds = 0)
{
    $read = [$pipe];
    $write = $except = null;
    return stream_select($read, $write, $except, $seconds, $microseconds);
}

/* The length of a sleep depends on the resolution of the system timer, so the
 * calls are compared with real sleeps. The fastest round is not affected by
 * a busy machine. */
function fastest(callable $callback)
{
    $times = [];
    for ($round = 0; $round < 10; $round++) {
        $start = hrtime(true);
        for ($i = 0; $i < 10; $i++) {
            $callback();
        }
        $times[] = hrtime(true) - $start;
    }
    return min($times);
}

echo "empty pipe\n";
$sleep = fastest(fn() => usleep(100));
$select = fastest(fn() => select($pipes[1], 0));
var_dump($select < $sleep / 2);
var_dump(select($pipes[1], 0));

echo "positive timeout still waits\n";
$start = hrtime(true);
var_dump(select($pipes[1], 0, 200000));
var_dump(hrtime(true) - $start > 100e6);

echo "data\n";
fwrite($pipes[0], "\n");
var_dump(select($pipes[1], 10), select($pipes[1], 0));
var_dump(fread($pipes[1], 10));
var_dump(select($pipes[1], 0));

echo "closed pipe\n";
fwrite($pipes[0], "\n");
var_dump(select($pipes[1], 10));
var_dump(fread($pipes[1], 10), feof($pipes[1]));
var_dump(select($pipes[1], 0));
?>
--EXPECT--
empty pipe
bool(true)
int(0)
positive timeout still waits
int(0)
bool(true)
data
int(1)
int(1)
string(1) "x"
int(0)
closed pipe
int(1)
string(0) ""
bool(true)
int(1)
