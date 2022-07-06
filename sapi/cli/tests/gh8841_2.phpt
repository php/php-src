--TEST--
GH-8841: Fix invalid return type compilation doesn't register function (with libedit)
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded('readline')) die("skip need readline support");
if (READLINE_LIB !== "libedit") die('skip libedit only');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$codes = array();

$codes[1] = <<<EOT
function f(\$x): void { return \$x; }
f(1);
EOT;

$codes[2] = <<<EOT
function f(\$x): void { return \$x; }
function f(\$x): int { return \$x; }
echo f(1);
EOT;

$codes[3] = <<<EOT
function foo() { return \$x[]; }
foo();
EOT;

foreach ($codes as $key => $code) {
    echo "\n--------------\nSnippet no. $key:\n--------------\n";
    $php = getenv('TEST_PHP_EXECUTABLE');
    $ini = getenv('TEST_PHP_EXTRA_ARGS');
    $descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
    $proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
    fwrite($pipes[0], $code);
    fclose($pipes[0]);
    proc_close($proc);
}
?>
--EXPECT--
--------------
Snippet no. 1:
--------------
Interactive shell


Fatal error: A void function must not return a value in php shell code on line 1

Warning: Uncaught Error: Call to undefined function f() in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1

--------------
Snippet no. 2:
--------------
Interactive shell


Fatal error: A void function must not return a value in php shell code on line 1
1

--------------
Snippet no. 3:
--------------
Interactive shell


Fatal error: Cannot use [] for reading in php shell code on line 1

Warning: Uncaught Error: Call to undefined function foo() in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1
