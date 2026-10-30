--TEST--
GH-9670 (php -h does not list the -C and -q options)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

foreach (explode("\n", `$php -n -h`) as $line) {
    if (preg_match('/^  -[Cq] /', $line)) {
        echo $line, "\n";
    }
}

?>
--EXPECT--
  -C               Do not chdir to the script's directory (no-op in CLI)
  -q               Do not output HTTP headers (no-op in CLI)
