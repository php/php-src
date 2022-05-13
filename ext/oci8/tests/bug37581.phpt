--TEST--
Bug #37581 (oci_bind_array_by_name clobbers input array when using SQLT_AFC, AVC)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

$p1 = "create or replace package BUG37581_PKG as
type str_array is table of char(2) index by binary_integer;
procedure array_bind(in_str in str_array, out_str out string);
end BUG37581_PKG;";

$p2 = "create or replace package body BUG37581_PKG as
  procedure array_bind(in_str in str_array, out_str out string) is
  begin
    for i in 1 .. in_str.count loop
      out_str := in_str(i);
    end loop;
  end array_bind;
end BUG37581_PKG;";

$s1 = oci_parse($c, $p1);
$s2 = oci_parse($c, $p2);
oci_execute($s1);
oci_execute($s2);


$stmt           = oci_parse($c,'begin bug37581_pkg.array_bind(:in_arr, :out_str); end;');
$strings        = array('A','B','C','D','E');

oci_bind_array_by_name($stmt,':in_arr',$strings,5,1,SQLT_AFC);
oci_bind_by_name($stmt,':out_str',$result,10);

oci_execute($stmt);
var_dump($strings);

oci_execute($stmt);
var_dump($strings);

echo "Done\n";
?>
--EXPECT--
array(5) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  string(1) "D"
  [4]=>
  string(1) "E"
}
array(5) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  string(1) "D"
  [4]=>
  string(1) "E"
}
Done
