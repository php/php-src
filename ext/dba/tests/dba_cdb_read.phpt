--TEST--
DBA CDB handler test (read only)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb_make');
?>
--CONFLICTS--
test.cdb
--FILE--
<?php
    echo "database handler: cdb\n";
    $handler = 'cdb';
    $db_file = __DIR__.'/test.cdb';
    if (($db_file=dba_open($db_file, "r", $handler))!==FALSE) {
        // read key sequence
        $a = dba_firstkey($db_file);
        $count= 0;
        $keys = $a;
        while($a) {
            $a = dba_nextkey($db_file);
            $keys .= $a;
            $count++;
        }
        // display number of entries and key existence
        echo $count;
        for ($i=1; $i<8; $i++) {
            echo dba_exists($i, $db_file) ? "Y" : "N";
        }
        echo "\n=";
        echo dba_fetch(1, $db_file);
        echo dba_fetch(2, $db_file);
        echo dba_fetch(3, $db_file);
        echo dba_fetch(4, $db_file);
        echo "\n#";
        echo dba_fetch(1, $db_file);
        echo dba_fetch(1, $db_file);
        echo dba_fetch(2, $db_file);
        echo dba_fetch(2, $db_file);
        echo "\n?".$keys;
        // with skip = 0 dba_fetch must fetch the first result
        echo "\n#";
        $skip = array();
        for ($i=0; $i < strlen($keys); $i++) {
            $key = substr($keys, $i, 1);
            $skip[$key] = 0;
            echo dba_fetch($key, $db_file);
        }
        echo "\n=";
        for ($i=0; $i < strlen($keys); $i++) {
            $key = substr($keys, $i, 1);
            echo dba_fetch($key, $db_file, $skip[$key]);
            $skip[$key]++;
        }
        dba_close($db_file);
    } else {
        echo "Error creating database\n";
    }
?>
--EXPECT--
database handler: cdb
7YYYYNNN
=1234
#1122
?1212314
#1212314
=1231324
