--TEST--
sqlite-oo: changes
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
?>
--FILE--
<?php 
include "blankdb_oo.inc";

$data = array("one", "two", "three");

$db->query("CREATE TABLE strings(a VARCHAR)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('$str')");
	echo $db->changes() . "\n";
}

$db->query("UPDATE strings SET a='foo' WHERE a!='two'");
echo $db->changes() . "\n";

$db->query("DELETE FROM strings WHERE 1");
echo $db->changes() . "\n";

$str = '';
foreach ($data as $s) {
	$str .= "INSERT INTO strings VALUES('".$s."');";
}
$db->query($str);
echo $db->changes() . "\n";

?>
--EXPECT--
1
1
1
2
3
3
