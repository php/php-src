--TEST--
DBA Handler Test
--EXTENSIONS--
dba
--SKIPIF--
<?php
$handler="flatfile";
require(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
$handler="flatfile";
require_once(__DIR__ .'/test.inc');
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

check(dba_handlers(false));

echo "Test 3\n";

check(dba_handlers(0));

echo "Test 4 - full info\n";
$h = dba_handlers(1);
foreach ($h as $key => $val) {
    if ($key === "flatfile") {
        echo "Success: flatfile enabled\n";
    }
}

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECT--
database handler: flatfile
Test 1
Success: flatfile enabled
Test 2
Success: flatfile enabled
Test 3
Success: flatfile enabled
Test 4 - full info
Success: flatfile enabled
