--TEST--
dbx_sort
--SKIPIF--
<?php 
include_once("skipif.inc");
?>
--FILE--
<?php 
include_once("dbx_test.p");
$sql_statement = "select id, description from tbl where parentid=1 order by id";
$compare_function = "cmp";
$invalid_compare_function = "invalid_cmp";
$nonexisting_compare_function = "nonexisting_cmp";
$dlo = dbx_connect($module, $host, $database, $username, $password);
function invalid_cmp() {
    return "blabla";
    }
function cmp($a, $b) {
    $rv = dbx_compare($a, $b, "description");
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
    if (dbx_sort($dro, $compare_function)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i]['id'].".".$dro->data[$i]['description']."\n");
            }
        }
    if (!@dbx_sort(0, $compare_function)) {
        print('wrong dbx_result_object: dbx_sort failure works ok'."\n");
        }
    if (dbx_sort($dro, $nonexisting_compare_function)) {
        print('nonexisting compare function: dbx_sort will NOT complain'."\n");
        }
    if (dbx_sort($dro, $invalid_compare_function)) {
        print('invalid compare function: dbx_sort will NOT complain'."\n");
        }
    if (!@dbx_sort($dro, $compare_function, "12many")) {
        print('too many parameters: dbx_sort failure works ok'."\n");
        }
    if (!@dbx_sort($dro)) {
        print('too few parameters: dbx_sort failure works ok'."\n");
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
wrong dbx_result_object: dbx_sort failure works ok
nonexisting compare function: dbx_sort will NOT complain
invalid compare function: dbx_sort will NOT complain
too many parameters: dbx_sort failure works ok
too few parameters: dbx_sort failure works ok