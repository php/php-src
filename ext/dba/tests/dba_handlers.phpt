--TEST--
DBA Handler Test
--SKIPIF--
<?php 
$handler="flatfile";
require(dirname(__FILE__) .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
$handler="flatfile";
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";

function check($h)
{
    if (!$h) {
        return;
    }

    foreach ($h as $key) {
        if ($key === "flatfile") {
            echo "Success: flatfile enabled\n";
        }
    }
}

echo "Test 1\n";

check(dba_handlers());

echo "Test 2\n";

check(dba_handlers(null));

echo "Test 3\n";

check(dba_handlers(1, 2));

echo "Test 4\n";

check(dba_handlers(0));

echo "Test 5 - full info\n";
$h = dba_handlers(1);
foreach ($h as $key => $val) {
    if ($key === "flatfile") {
        echo "Success: flatfile enabled\n";
    }
}

?>
--CLEAN--
<?php 
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTF--
database handler: flatfile
Test 1
Success: flatfile enabled
Test 2
Success: flatfile enabled
Test 3

Warning: dba_handlers() expects at most 1 parameter, 2 given in %sdba_handlers.php on line %d
Test 4
Success: flatfile enabled
Test 5 - full info
Success: flatfile enabled
