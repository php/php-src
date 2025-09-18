--TEST--
syntax check
--SKIPIF--
<?php include "skipif.inc"; ?>
--INI--
display_errors=stdout
--FILE--
<?php
include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$filename = __DIR__."/006.test.php";

$code = '
<?php

$test = "var";

class test {
    private $var;
}

echo test::$var;

?>
';

file_put_contents($filename, $code);

var_dump(shell_exec(<<<SHELL
"$php" -n -l "$filename"
SHELL));
var_dump(shell_exec(<<<SHELL
"$php" -n -l some.unknown
SHELL));

$code = '
<?php

class test
    private $var;
}

?>
';

file_put_contents($filename, $code);

if (defined("PHP_WINDOWS_VERSION_MAJOR")) {
    var_dump(shell_exec(<<<SHELL
    "$php" -n -l "$filename"
    SHELL));
} else {
    var_dump(shell_exec(<<<SHELL
    "$php" -n -l "$filename" 2>/dev/null
    SHELL));
}

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--
string(%d) "No syntax errors detected in %s006.test.php
"
string(%d) "No input file specified.
"
string(%d) "<br />
<b>Parse error</b>: %s expecting %s{%s in <b>%s006.test.php</b> on line <b>5</b><br />
Errors parsing %s006.test.php
"
Done
