--TEST--
DBA DB4 Handler Test
--SKIPIF--
<?php 
$handler="db4";
require(dirname(__FILE__) .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
$handler="db4";
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";

function check($h)
{
    if (!$h) {
        return;
    }

    foreach ($h as $key) {
        if ($key === "db4") {
            echo "Success: db4 enabled\n";
        }
    }
}

echo "Test 1\n";

check(dba_handlers());

echo "Test 2 - full info\n";
$h = dba_handlers(1);
foreach ($h as $key => $val) {
    if ($key === "db4") {
        echo "$val\n";
    }
}

?>
--CLEAN--
<?php 
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTREGEX--
database handler: db4
Test 1
Success: db4 enabled
Test 2 - full info
.*Berkeley DB (4|5).*
