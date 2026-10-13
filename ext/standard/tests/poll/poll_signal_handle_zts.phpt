--TEST--
Io\Poll\SignalHandle is refused outside the CLI in thread-safe builds
--CGI--
--SKIPIF--
<?php
if (!PHP_ZTS) die("skip thread-safe builds only");
?>
--FILE--
<?php
try {
    new Io\Poll\SignalHandle([10]);
} catch (Io\Poll\PollException $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Io\Poll\PollException: Io\Poll\SignalHandle is only available in the CLI in thread-safe builds
