--TEST--
oci_execute() segfault after repeated bind of LOB descriptor
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob, clob)
                      VALUES (empty_blob(), empty_clob())
                      RETURNING
                               clob
                      INTO :v_clob ";

$s = oci_parse($c, $ora_sql);
$clob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ":v_clob", $clob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($clob->save("some text data"));

oci_bind_by_name($s, ":v_clob", $clob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($clob->save("some more text data"));

$query = 'SELECT clob, DBMS_LOB.GETLENGTH(clob) FROM '.$schema.$table_name.' ORDER BY 2';

$s = oci_parse ($c, $query);
oci_execute($s, OCI_DEFAULT);

while ($arr = oci_fetch_assoc($s)) {
    $result = $arr['CLOB']->load();
    var_dump($result);
}

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
bool(true)
bool(true)
unicode(14) "some text data"
unicode(19) "some more text data"
Done
