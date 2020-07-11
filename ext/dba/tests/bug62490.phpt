--TEST--
Bug #62490 (dba_delete returns true on missing item (inifile))
--SKIPIF--
<?php
$handler = "inifile";
include "skipif.inc";
?>
--FILE--
<?php
$handler = "inifile";
include "test.inc";

$dba = dba_open($db_filename, "n", $handler)
    or die;
for ($i = 0; $i < 3; ++$i) {
    echo "insert $i:";
    var_dump(dba_insert("a", $i, $dba));
}

echo "exists:";
var_dump(dba_exists("a", $dba));
echo "delete:";
var_dump(dba_delete("a", $dba));
echo "exists:";
var_dump(dba_exists("a", $dba));
echo "delete:";
var_dump(dba_delete("a", $dba));

?>
--CLEAN--
<?php
include "clean.inc";
?>
--EXPECT--
insert 0:bool(true)
insert 1:bool(true)
insert 2:bool(true)
exists:bool(true)
delete:bool(true)
exists:bool(false)
delete:bool(false)
