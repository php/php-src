--TEST--
GH-14189 (PHP Interactive shell input state incorrectly handles quoted heredoc literals.)
--EXTENSIONS--
readline
--SKIPIF--
<?php
include "skipif.inc";
if (readline_info('done') === NULL) {
    die ("skip need readline support");
}
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');

// disallow console escape sequences that may break the output
putenv('TERM=VT100');

$code = <<<EOT
\$test = <<<"EOF"
foo
bar
baz
EOF;
echo \$test;
exit
EOT;

$code = escapeshellarg($code);
echo `echo $code | "$php" -a`, "\n";
?>
--EXPECT--
Interactive shell

php > $test = <<<"EOF"
<<< > foo
<<< > bar
<<< > baz
<<< > EOF;
php > echo $test;
foo
bar
baz
php > exit
