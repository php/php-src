--TEST--
Bug #35973 (Error ORA-24806 occurs when trying to fetch a NCLOB field)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$s1 = oci_parse($c, "drop table test_nclob");
@oci_execute($s1);

$s2 = oci_parse($c, "create table test_nclob (nc NCLOB)");
oci_execute($s2);

$s3 = oci_parse($c, "insert into test_nclob (nc) values ('12345data')");
oci_execute($s3);

$s3 = oci_parse($c, "select * from test_nclob");
oci_execute($s3);

var_dump($data = oci_fetch_assoc($s3));
$d = $data['NC'];

var_dump($d->read(5));
var_dump($d->read(4));

$s1 = oci_parse($c, "drop table test_nclob");
@oci_execute($s1);

echo "Done\n";
?>
--EXPECTF--
array(1) {
  ["NC"]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(%d) "%s5"
string(%d) "%sa"
Done
