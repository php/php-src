--TEST--
multiple files syntax check
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

function run_and_output($cmd) {
    exec($cmd, $output, $exit_code);
    print_r($output);
    var_dump($exit_code);
}

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

$filename_good = __DIR__."/024_good.test.php";
$filename_good_escaped = escapeshellarg($filename_good);
$filename_bad = __DIR__."/024_bad.test.php";
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

run_and_output("$php -n -l $filename_good_escaped $filename_good_escaped 2>&1");
run_and_output("$php -n -l $filename_good_escaped some.unknown $filename_good_escaped 2>&1");
run_and_output("$php -n -l $filename_good_escaped $filename_bad_escaped $filename_good_escaped 2>&1");
run_and_output("$php -n -l $filename_bad_escaped $filename_bad_escaped 2>&1");
run_and_output("$php -n -l $filename_bad_escaped some.unknown $filename_bad_escaped 2>&1");
run_and_output("$php -n -l $filename_bad_escaped $filename_bad_escaped some.unknown 2>&1");

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
    [0] => No syntax errors detected in %s024_good.test.php
    [1] => No syntax errors detected in %s024_good.test.php
)
int(0)
Array
(
    [0] => No syntax errors detected in %s024_good.test.php
    [1] => Could not open input file: some.unknown
    [2] => No syntax errors detected in %s024_good.test.php
)
int(1)
Array
(
    [0] => No syntax errors detected in %s024_good.test.php
    [1] => 
    [2] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [3] => Errors parsing %s024_bad.test.php
    [4] => No syntax errors detected in %s024_good.test.php
)
int(255)
Array
(
    [0] => 
    [1] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [2] => Errors parsing %s024_bad.test.php
    [3] => 
    [4] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [5] => Errors parsing %s024_bad.test.php
)
int(255)
Array
(
    [0] => 
    [1] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [2] => Errors parsing %s024_bad.test.php
    [3] => Could not open input file: some.unknown
    [4] => 
    [5] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [6] => Errors parsing %s024_bad.test.php
)
int(255)
Array
(
    [0] => 
    [1] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [2] => Errors parsing %s024_bad.test.php
    [3] => 
    [4] => Parse error: syntax error, unexpected token "private", expecting "{" in %s on line %d
    [5] => Errors parsing %s024_bad.test.php
    [6] => Could not open input file: some.unknown
)
int(1)
Done
