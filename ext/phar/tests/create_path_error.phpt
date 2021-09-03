--TEST--
Phar: create with illegal path
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php?', "query");
file_put_contents($pname . '/b.php?bla', "query");

var_dump(file_get_contents($pname . '/a.php'));
var_dump(file_get_contents($pname . '/b.php'));

function error_handler($errno, $errmsg)
{
    echo "Error: $errmsg";
}

set_error_handler('error_handler');

$count = 0;
$checks = array(
    '/', '.', '../', 'a/..', 'a/', 'b//a.php',
    "Font\xE5\x84\xB7\xE9\xBB\x91pro.ttf", //two valid multi-byte characters
    "\xF0\x9F\x98\x8D.ttf", // valid 4 byte char - smiling face with heart-shaped eyes
    "Font\xE9\xBBpro.ttf", //Invalid multi-byte character - missing last byte
    "Font\xBB\x91pro.ttf",   //Invalid multi-byte character - missing first byte
    "Font\xC0\xAFpro.ttf",   //Invalid multi-byte character - invalid first byte
    "Font\xF0\x80\x90\x90pro.ttf",   //Invalid multi-byte character - surrogate pair code point
    "\xFC\x81\x81\x81\x81pro.ttf", //RFC 3629 limited char points to 0000-10FFFF aka 5 byte utf-8 not valid
);
foreach($checks as $check)
{
    $count++;
    echo "$count:";
    file_put_contents($pname . '/' . $check, "error");
    echo "\n";
}

$phar = new Phar($fname);
$checks = array("a\0");
foreach($checks as $check)
{
    try
    {
        $phar[$check] = 'error';
    }
    catch (ValueError $e)
    {
        echo 'Exception: ' . $e->getMessage() . "\n";
    }
}

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
string(5) "query"
string(5) "query"
1:Error: file_put_contents(phar://%s//): Failed to open stream: phar error: file "" in phar "%s" cannot be empty
2:Error: file_put_contents(phar://%s/.): Failed to open stream: phar error: file "" in phar "%s" cannot be empty
3:Error: file_put_contents(phar://%s/../): Failed to open stream: phar error: file "" in phar "%s" cannot be empty
4:Error: file_put_contents(phar://%s/a/..): Failed to open stream: phar error: file "" in phar "%s" cannot be empty
5:
6:
7:
8:
9:Error: file_put_contents(phar://%s): Failed to open stream: phar error: invalid path "%s" contains illegal character
10:Error: file_put_contents(phar://%s): Failed to open stream: phar error: invalid path "%s" contains illegal character
11:Error: file_put_contents(phar://%s): Failed to open stream: phar error: invalid path "%s" contains illegal character
12:Error: file_put_contents(phar://%s): Failed to open stream: phar error: invalid path "%s" contains illegal character
13:Error: file_put_contents(phar://%s): Failed to open stream: phar error: invalid path "%s" contains illegal character
Exception: Phar::offsetSet(): Argument #1 ($localName) must not contain any null bytes
