--TEST--
LOB prefetching
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';
require __DIR__.'/create_table.inc';

define("NUMROWS", 200);
define("LOBSIZE", 64000);

$ora_sql =
  "declare
    c clob;
    b blob;
    numrows number := " . NUMROWS . ";
    dest_offset integer := 1;
    src_offset  integer := 1;
    warn        integer;
    ctx         integer := dbms_lob.default_lang_ctx;
  begin
    for j in 1..numrows
    loop
      c := DBMS_RANDOM.string('L',TRUNC(DBMS_RANDOM.value(1000,1000)));
      for i in 1..6
      loop
        c := c||c;
      end loop;
      dbms_lob.createtemporary(b, false);
      dbms_lob.converttoblob(b, c, dbms_lob.lobmaxsize, dest_offset, src_offset, dbms_lob.default_csid, ctx, warn);
      insert /*+ APPEND */ into {$schema}{$table_name} (id, clob, blob) values (j, c, b);
    end loop;
    commit;
  end;";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


function get_clob_loc($c, $sql, $pfl) {
    $stid = oci_parse($c, $sql);
    if ($pfl >= 0)
        oci_set_prefetch_lob($stid, $pfl);
    oci_execute($stid);
    $l = [];
    while (($row = oci_fetch_array($stid, OCI_ASSOC)) != false) {
        $l[] = $row['CLOB']->load();
        $row['CLOB']->free();
        if (strlen($l[0]) != LOBSIZE) { print("strlen(l) failure" .  strlen($l)); exit; }
    }
    return($l);
}

function get_clob_inline($c, $sql, $pfl) {
    $stid = oci_parse($c, $sql);
    if ($pfl >= 0)
        oci_set_prefetch_lob($stid, $pfl);
    oci_execute($stid);
    $l = [];
    while (($row = oci_fetch_array($stid, OCI_ASSOC+OCI_RETURN_LOBS)) != false) {
        $l[] = $row['CLOB'];
        if (strlen($l[0]) != LOBSIZE) { print("strlen(l) failure" . strlen($l)); exit; }
    }
    return($l);
}

function check_clobs($locarr, $inlinearr) {
    print("Comparing CLOBS\n");
    for ($i = 0; $i < NUMROWS; ++$i) {
        if (strlen($locarr[$i]) != LOBSIZE) {
            trigger_error("size mismatch at $i " . strlen($locarr[$i]), E_USER_ERROR);
            exit;
        }
        if (strcmp($locarr[$i], $inlinearr[$i])) {
            trigger_error("data mismatch at $i " . strlen($locarr[$i]) . " " . strlen($inlinearr[$i]), E_USER_ERROR);
            exit;
        }
    }
}

function get_blob_loc($c, $sql, $pfl) {
    $stid = oci_parse($c, $sql);
    if ($pfl >= 0)
        oci_set_prefetch_lob($stid, $pfl);
    oci_execute($stid);
    $l = [];
    while (($row = oci_fetch_array($stid, OCI_ASSOC)) != false) {
        $l[] = $row['BLOB']->load();
        $row['BLOB']->free();
        if (strlen($l[0]) != LOBSIZE) { print("strlen(l) failure" .  strlen($l)); exit; }
    }
    return($l);
}


print("Test 1 - Default prefetch_lob_size\n");

$r = ini_get("oci8.prefetch_lob_size");
var_dump($r);

print("Test 2\n");

$s = oci_parse($c, 'select * from dual');
$r = oci_set_prefetch_lob($s, 0);
var_dump($r);

try {
    oci_set_prefetch_lob($s, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

print("Test 3 - CLOB prefetch_lob_size 100000\n");

$sql = "select clob from {$schema}{$table_name}" . " order by id";
$locarr = get_clob_loc($c, $sql, 100000);
$inlinearr = get_clob_inline($c, $sql, 100000);

print(count($locarr) . "\n");
print(count($inlinearr) . "\n");
check_clobs($locarr, $inlinearr);

print("Test 3 - CLOB prefetch_lob_size 100\n");

$locarr = get_clob_loc($c, $sql, 100);
$inlinearr = get_clob_inline($c, $sql, 100);

print(count($locarr) . "\n");
print(count($inlinearr) . "\n");
check_clobs($locarr, $inlinearr);

print("Test 4 - BLOB prefetch_lob_size 100000\n");

$sql = "select blob from {$schema}{$table_name}" . " order by id";
$locarr = get_blob_loc($c, $sql, 100000);

print(count($locarr) . "\n");

require __DIR__.'/drop_table.inc';

?>
DONE
--EXPECTF--
Test 1 - Default prefetch_lob_size
string(1) "0"
Test 2
bool(true)
oci_set_prefetch_lob(): Argument #2 ($prefetch_lob_size) must be greater than or equal to 0
Test 3 - CLOB prefetch_lob_size 100000
200
200
Comparing CLOBS
Test 3 - CLOB prefetch_lob_size 100
200
200
Comparing CLOBS
Test 4 - BLOB prefetch_lob_size 100000
200
DONE
