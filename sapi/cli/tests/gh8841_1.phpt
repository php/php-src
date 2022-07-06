--TEST--
GH-8841: Fix invalid return type compilation doesn't register function (without libedit)
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded('readline') || READLINE_LIB !== "readline") {
    die ("skip need readline support");
}
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');

// disallow console escape sequences that may break the output
putenv('TERM=VT100');

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
    $code = escapeshellarg($code);
    echo `echo $code | "$php" -a`, "\n";
}

echo "\nDone\n";
?>
--EXPECT--
--------------
Snippet no. 1:
--------------
Interactive shell

php > function f($x): void { return $x; }

Fatal error: A void function must not return a value in php shell code on line 1
php > f(1);

Warning: Uncaught Error: Call to undefined function f() in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1
php > 

--------------
Snippet no. 2:
--------------
Interactive shell

php > function f($x): void { return $x; }

Fatal error: A void function must not return a value in php shell code on line 1
php > function f($x): int { return $x; }
php > echo f(1);
1
php > 

--------------
Snippet no. 3:
--------------
Interactive shell

php > function foo() { return $x[]; }

Fatal error: Cannot use [] for reading in php shell code on line 1
php > foo();

Warning: Uncaught Error: Call to undefined function foo() in php shell code:1
Stack trace:
#0 {main}
  thrown in php shell code on line 1
php > 

Done
