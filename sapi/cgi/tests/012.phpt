--TEST--
multiple files syntax check
--SKIPIF--
<?php include "skipif.inc"; ?>
--INI--
display_errors=stdout
--FILE--
<?php
include "include.inc";

function run_and_output($cmd) {
    if (!defined("PHP_WINDOWS_VERSION_MAJOR")) {
        $cmd .= " 2>/dev/null";
    }
    exec($cmd, $output, $exit_code);
    print_r($output);
    // Normalize Windows vs Linux exit codes. On Windows exit code -1 is actually -1 instead of 255.
    if ($exit_code < 0) {
        $exit_code += 256;
    }
    var_dump($exit_code);
}

$php = get_cgi_path();
reset_env_vars();

$filename_good = __DIR__."/012_good.test.php";
$filename_good_escaped = escapeshellarg($filename_good);
$filename_bad = __DIR__."/012_bad.test.php";
$filename_bad_escaped = escapeshellarg($filename_bad);

$code = '
<?php

echo "hi";
';

file_put_contents($filename_good, $code);

$code = '
<?php

class test
    private $var;
}

?>
';

file_put_contents($filename_bad, $code);

run_and_output("$php -n -l $filename_good_escaped $filename_good_escaped");
run_and_output("$php -n -l $filename_good_escaped some.unknown $filename_good_escaped");
run_and_output("$php -n -l $filename_good_escaped $filename_bad_escaped $filename_good_escaped");
run_and_output("$php -n -l $filename_bad_escaped $filename_bad_escaped");
run_and_output("$php -n -l $filename_bad_escaped some.unknown $filename_bad_escaped");
run_and_output("$php -n -l $filename_bad_escaped $filename_bad_escaped some.unknown");

echo "Done\n";
?>
--CLEAN--
<?php
@unlink($filename_good);
@unlink($filename_bad);
?>
--EXPECTF--
Array
(
    [0] => No syntax errors detected in %s012_good.test.php
    [1] => No syntax errors detected in %s012_good.test.php
)
int(0)
Array
(
    [0] => No syntax errors detected in %s012_good.test.php
    [1] => No input file specified.
)
int(255)
Array
(
    [0] => No syntax errors detected in %s012_good.test.php
    [1] => <br />
    [2] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [3] => Errors parsing %s012_bad.test.php
    [4] => No syntax errors detected in %s012_good.test.php
)
int(255)
Array
(
    [0] => <br />
    [1] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [2] => Errors parsing %s012_bad.test.php
    [3] => <br />
    [4] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [5] => Errors parsing %s012_bad.test.php
)
int(255)
Array
(
    [0] => <br />
    [1] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [2] => Errors parsing %s012_bad.test.php
    [3] => No input file specified.
)
int(255)
Array
(
    [0] => <br />
    [1] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [2] => Errors parsing %s012_bad.test.php
    [3] => <br />
    [4] => <b>Parse error</b>:  syntax error, unexpected token &quot;private&quot;, expecting &quot;{&quot; in <b>%s012_bad.test.php</b> on line <b>5</b><br />
    [5] => Errors parsing %s012_bad.test.php
    [6] => No input file specified.
)
int(255)
Done
