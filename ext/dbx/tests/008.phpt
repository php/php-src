--TEST--
dbx_compare
--SKIPIF--
<?php 
include_once("skipif.inc");
?>
--FILE--
<?php 
include_once("dbx_test.p");
$sql_statement = "select id, description from tbl where parentid=1 order by id";
$compare_function_1 = "cmp_description_id";
$compare_function_2 = "cmp_description_desc_id";
$compare_function_3 = "cmp_description_id_desc";
$compare_function_4 = "cmp_description_desc_id_desc";
$compare_function_5 = "cmp_description_txt_id";
$compare_function_6 = "cmp_description_number_id";
$dlo = dbx_connect($module, $host, $database, $username, $password);
function cmp_description_id($a, $b) {
    $rv = dbx_compare($a, $b, "description");
    if (!$rv) $rv = dbx_compare($a, $b, "id");
    return $rv;
    }
function cmp_description_desc_id($a, $b) {
    $rv = dbx_compare($a, $b, "description", DBX_CMP_DESC);
    if (!$rv) $rv = dbx_compare($a, $b, "id");
    return $rv;
    }
function cmp_description_id_desc($a, $b) {
    $rv = dbx_compare($a, $b, "description");
    if (!$rv) $rv = dbx_compare($a, $b, "id", DBX_CMP_DESC);
    return $rv;
    }
function cmp_description_desc_id_desc($a, $b) {
    $rv = dbx_compare($a, $b, "description", DBX_CMP_DESC);
    if (!$rv) $rv = dbx_compare($a, $b, "id", DBX_CMP_DESC);
    return $rv;
    }
function cmp_description_txt_id($a, $b) {
    $rv = dbx_compare($a, $b, "description", DBX_CMP_TEXT);
    if (!$rv) $rv = dbx_compare($a, $b, "id");
    return $rv;
    }
function cmp_description_number_id($a, $b) {
    $rv = dbx_compare($a, $b, "description", DBX_CMP_NUMBER);
    if (!$rv) $rv = dbx_compare($a, $b, "id");
    return $rv;
    }
if (!$dlo) {
    print('this won\'t work'."\n");
	}
else {
    $dro = dbx_query($dlo, $sql_statement);
    if (!$dro) {
        print('this won\'t work'."\n");
        }
    for ($i=0; $i<$dro->rows; ++$i) {
        print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
        }
    if (dbx_sort($dro, $compare_function_1)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (dbx_sort($dro, $compare_function_2)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (dbx_sort($dro, $compare_function_3)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (dbx_sort($dro, $compare_function_4)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (dbx_sort($dro, $compare_function_5)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (dbx_sort($dro, $compare_function_6)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (!@dbx_compare($a, $b, "fieldname")) {
        print('wrong parameters: dbx_compare failure works ok'."\n");
        }
    if (!@dbx_compare($a, $b, "fieldname", DBX_CMP_NATIVE, "12many")) {
        print('too many parameters: dbx_compare failure works ok'."\n");
        }
    if (!@dbx_compare($a, $b)) {
        print('too few parameters: dbx_compare failure works ok'."\n");
        }
    dbx_close($dlo);
    }
?>
--EXPECT--
10.abc
20.cba
30.bac
40.100
50.20
60.20
40.100
50.20
60.20
10.abc
30.bac
20.cba
20.cba
30.bac
10.abc
50.20
60.20
40.100
40.100
60.20
50.20
10.abc
30.bac
20.cba
20.cba
30.bac
10.abc
60.20
50.20
40.100
40.100
50.20
60.20
10.abc
30.bac
20.cba
10.0
20.0
30.0
50.20
60.20
40.100
wrong parameters: dbx_compare failure works ok
too many parameters: dbx_compare failure works ok
too few parameters: dbx_compare failure works ok