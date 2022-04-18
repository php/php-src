--TEST--
PL/SQL: dbms_output
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace procedure dbmsoutput_proc as
      begin
        dbms_output.put_line('Hello World!');
      end;",

    "create or replace type dorow as table of varchar2(4000)",

    "create or replace function mydofetch return dorow pipelined is
    line    varchar2(4000);
    status  integer;
    begin
      loop
        dbms_output.get_line(line, status);
        exit when status = 1;
        pipe row (line);
      end loop;
      return;
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

// Turn DBMS_OUTPUT on
function setserveroutputon($c)
{
    $s = oci_parse($c, "begin dbms_output.enable(null); end;");
    oci_execute($s);
}

// Create some output
function createoutput($c, $prefix)
{
    $s = oci_parse($c, "call dbms_output.put_line(:bv1 || ' ' || :bv2 || ' Hello, world! Lots and lots and ... of text')");
    oci_bind_by_name($s, ":bv1", $i, -1, SQLT_INT);
    oci_bind_by_name($s, ":bv2", $prefix);
    for ($i = 0; $i < 100; ++$i) {
        oci_execute($s);
    }
}

// Call dbms_output.get_line()
// Returns an array of DBMS_OUTPUT lines, or false.
function getdbmsoutput_do($c)
{
    $s = oci_parse($c, "begin dbms_output.get_line(:ln, :st); end;");
    oci_bind_by_name($s, ":ln", $ln, 100);
    oci_bind_by_name($s, ":st", $st, -1, SQLT_INT);
    $res = [];
    while (($succ = oci_execute($s)) && !$st) {
        $res[] = $ln;  // append each line to the array
    }
    return $res;
}

function getdbmsoutput_do2($c)
{
    $orignumlines = $numlines = 100;
    $s = oci_parse($c, "begin dbms_output.get_lines(:lines, :numlines); end;");
    $r = oci_bind_by_name($s, ":numlines", $numlines);
    $res = array();
    while ($numlines >= $orignumlines) {
        oci_bind_array_by_name($s, ":lines", $lines,  $numlines, 255, SQLT_CHR);
        oci_execute($s);
        if ($numlines == 0) {
            break;
        }
        $res = array_merge($res, array_slice($lines, 0, $numlines));
        unset($lines);
    }
    return $res;
}

function getdbmsoutput_pl($c)
{
    $s = oci_parse($c, "select * from table(mydofetch())");
    oci_execute($s);
    $res = [];
    while ($row = oci_fetch_array($s, OCI_NUM)) {
        $res[] = $row[0];
    }
    return $res;
}

echo "Test 1\n";

setserveroutputon($c);   // Turn output buffering on

$s = oci_parse($c, 'call dbmsoutput_proc()');
oci_execute($s);
var_dump(getdbmsoutput_do($c));

echo "Test 2\n";

createoutput($c, 'test 2');
var_dump(getdbmsoutput_do($c));

echo "Test 3\n";

createoutput($c, 'test 3');
var_dump(getdbmsoutput_do2($c));

echo "Test 4\n";

createoutput($c, 'test 4');
var_dump(getdbmsoutput_pl($c));

// Clean up

$stmtarray = array(
    "drop procedure dbmsoutput_proc"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
array(1) {
  [0]=>
  string(12) "Hello World!"
}
Test 2
array(100) {
  [0]=>
  string(52) "0 test 2 Hello, world! Lots and lots and ... of text"
  [1]=>
  string(52) "1 test 2 Hello, world! Lots and lots and ... of text"
  [2]=>
  string(52) "2 test 2 Hello, world! Lots and lots and ... of text"
  [3]=>
  string(52) "3 test 2 Hello, world! Lots and lots and ... of text"
  [4]=>
  string(52) "4 test 2 Hello, world! Lots and lots and ... of text"
  [5]=>
  string(52) "5 test 2 Hello, world! Lots and lots and ... of text"
  [6]=>
  string(52) "6 test 2 Hello, world! Lots and lots and ... of text"
  [7]=>
  string(52) "7 test 2 Hello, world! Lots and lots and ... of text"
  [8]=>
  string(52) "8 test 2 Hello, world! Lots and lots and ... of text"
  [9]=>
  string(52) "9 test 2 Hello, world! Lots and lots and ... of text"
  [10]=>
  string(53) "10 test 2 Hello, world! Lots and lots and ... of text"
  [11]=>
  string(53) "11 test 2 Hello, world! Lots and lots and ... of text"
  [12]=>
  string(53) "12 test 2 Hello, world! Lots and lots and ... of text"
  [13]=>
  string(53) "13 test 2 Hello, world! Lots and lots and ... of text"
  [14]=>
  string(53) "14 test 2 Hello, world! Lots and lots and ... of text"
  [15]=>
  string(53) "15 test 2 Hello, world! Lots and lots and ... of text"
  [16]=>
  string(53) "16 test 2 Hello, world! Lots and lots and ... of text"
  [17]=>
  string(53) "17 test 2 Hello, world! Lots and lots and ... of text"
  [18]=>
  string(53) "18 test 2 Hello, world! Lots and lots and ... of text"
  [19]=>
  string(53) "19 test 2 Hello, world! Lots and lots and ... of text"
  [20]=>
  string(53) "20 test 2 Hello, world! Lots and lots and ... of text"
  [21]=>
  string(53) "21 test 2 Hello, world! Lots and lots and ... of text"
  [22]=>
  string(53) "22 test 2 Hello, world! Lots and lots and ... of text"
  [23]=>
  string(53) "23 test 2 Hello, world! Lots and lots and ... of text"
  [24]=>
  string(53) "24 test 2 Hello, world! Lots and lots and ... of text"
  [25]=>
  string(53) "25 test 2 Hello, world! Lots and lots and ... of text"
  [26]=>
  string(53) "26 test 2 Hello, world! Lots and lots and ... of text"
  [27]=>
  string(53) "27 test 2 Hello, world! Lots and lots and ... of text"
  [28]=>
  string(53) "28 test 2 Hello, world! Lots and lots and ... of text"
  [29]=>
  string(53) "29 test 2 Hello, world! Lots and lots and ... of text"
  [30]=>
  string(53) "30 test 2 Hello, world! Lots and lots and ... of text"
  [31]=>
  string(53) "31 test 2 Hello, world! Lots and lots and ... of text"
  [32]=>
  string(53) "32 test 2 Hello, world! Lots and lots and ... of text"
  [33]=>
  string(53) "33 test 2 Hello, world! Lots and lots and ... of text"
  [34]=>
  string(53) "34 test 2 Hello, world! Lots and lots and ... of text"
  [35]=>
  string(53) "35 test 2 Hello, world! Lots and lots and ... of text"
  [36]=>
  string(53) "36 test 2 Hello, world! Lots and lots and ... of text"
  [37]=>
  string(53) "37 test 2 Hello, world! Lots and lots and ... of text"
  [38]=>
  string(53) "38 test 2 Hello, world! Lots and lots and ... of text"
  [39]=>
  string(53) "39 test 2 Hello, world! Lots and lots and ... of text"
  [40]=>
  string(53) "40 test 2 Hello, world! Lots and lots and ... of text"
  [41]=>
  string(53) "41 test 2 Hello, world! Lots and lots and ... of text"
  [42]=>
  string(53) "42 test 2 Hello, world! Lots and lots and ... of text"
  [43]=>
  string(53) "43 test 2 Hello, world! Lots and lots and ... of text"
  [44]=>
  string(53) "44 test 2 Hello, world! Lots and lots and ... of text"
  [45]=>
  string(53) "45 test 2 Hello, world! Lots and lots and ... of text"
  [46]=>
  string(53) "46 test 2 Hello, world! Lots and lots and ... of text"
  [47]=>
  string(53) "47 test 2 Hello, world! Lots and lots and ... of text"
  [48]=>
  string(53) "48 test 2 Hello, world! Lots and lots and ... of text"
  [49]=>
  string(53) "49 test 2 Hello, world! Lots and lots and ... of text"
  [50]=>
  string(53) "50 test 2 Hello, world! Lots and lots and ... of text"
  [51]=>
  string(53) "51 test 2 Hello, world! Lots and lots and ... of text"
  [52]=>
  string(53) "52 test 2 Hello, world! Lots and lots and ... of text"
  [53]=>
  string(53) "53 test 2 Hello, world! Lots and lots and ... of text"
  [54]=>
  string(53) "54 test 2 Hello, world! Lots and lots and ... of text"
  [55]=>
  string(53) "55 test 2 Hello, world! Lots and lots and ... of text"
  [56]=>
  string(53) "56 test 2 Hello, world! Lots and lots and ... of text"
  [57]=>
  string(53) "57 test 2 Hello, world! Lots and lots and ... of text"
  [58]=>
  string(53) "58 test 2 Hello, world! Lots and lots and ... of text"
  [59]=>
  string(53) "59 test 2 Hello, world! Lots and lots and ... of text"
  [60]=>
  string(53) "60 test 2 Hello, world! Lots and lots and ... of text"
  [61]=>
  string(53) "61 test 2 Hello, world! Lots and lots and ... of text"
  [62]=>
  string(53) "62 test 2 Hello, world! Lots and lots and ... of text"
  [63]=>
  string(53) "63 test 2 Hello, world! Lots and lots and ... of text"
  [64]=>
  string(53) "64 test 2 Hello, world! Lots and lots and ... of text"
  [65]=>
  string(53) "65 test 2 Hello, world! Lots and lots and ... of text"
  [66]=>
  string(53) "66 test 2 Hello, world! Lots and lots and ... of text"
  [67]=>
  string(53) "67 test 2 Hello, world! Lots and lots and ... of text"
  [68]=>
  string(53) "68 test 2 Hello, world! Lots and lots and ... of text"
  [69]=>
  string(53) "69 test 2 Hello, world! Lots and lots and ... of text"
  [70]=>
  string(53) "70 test 2 Hello, world! Lots and lots and ... of text"
  [71]=>
  string(53) "71 test 2 Hello, world! Lots and lots and ... of text"
  [72]=>
  string(53) "72 test 2 Hello, world! Lots and lots and ... of text"
  [73]=>
  string(53) "73 test 2 Hello, world! Lots and lots and ... of text"
  [74]=>
  string(53) "74 test 2 Hello, world! Lots and lots and ... of text"
  [75]=>
  string(53) "75 test 2 Hello, world! Lots and lots and ... of text"
  [76]=>
  string(53) "76 test 2 Hello, world! Lots and lots and ... of text"
  [77]=>
  string(53) "77 test 2 Hello, world! Lots and lots and ... of text"
  [78]=>
  string(53) "78 test 2 Hello, world! Lots and lots and ... of text"
  [79]=>
  string(53) "79 test 2 Hello, world! Lots and lots and ... of text"
  [80]=>
  string(53) "80 test 2 Hello, world! Lots and lots and ... of text"
  [81]=>
  string(53) "81 test 2 Hello, world! Lots and lots and ... of text"
  [82]=>
  string(53) "82 test 2 Hello, world! Lots and lots and ... of text"
  [83]=>
  string(53) "83 test 2 Hello, world! Lots and lots and ... of text"
  [84]=>
  string(53) "84 test 2 Hello, world! Lots and lots and ... of text"
  [85]=>
  string(53) "85 test 2 Hello, world! Lots and lots and ... of text"
  [86]=>
  string(53) "86 test 2 Hello, world! Lots and lots and ... of text"
  [87]=>
  string(53) "87 test 2 Hello, world! Lots and lots and ... of text"
  [88]=>
  string(53) "88 test 2 Hello, world! Lots and lots and ... of text"
  [89]=>
  string(53) "89 test 2 Hello, world! Lots and lots and ... of text"
  [90]=>
  string(53) "90 test 2 Hello, world! Lots and lots and ... of text"
  [91]=>
  string(53) "91 test 2 Hello, world! Lots and lots and ... of text"
  [92]=>
  string(53) "92 test 2 Hello, world! Lots and lots and ... of text"
  [93]=>
  string(53) "93 test 2 Hello, world! Lots and lots and ... of text"
  [94]=>
  string(53) "94 test 2 Hello, world! Lots and lots and ... of text"
  [95]=>
  string(53) "95 test 2 Hello, world! Lots and lots and ... of text"
  [96]=>
  string(53) "96 test 2 Hello, world! Lots and lots and ... of text"
  [97]=>
  string(53) "97 test 2 Hello, world! Lots and lots and ... of text"
  [98]=>
  string(53) "98 test 2 Hello, world! Lots and lots and ... of text"
  [99]=>
  string(53) "99 test 2 Hello, world! Lots and lots and ... of text"
}
Test 3
array(100) {
  [0]=>
  string(52) "0 test 3 Hello, world! Lots and lots and ... of text"
  [1]=>
  string(52) "1 test 3 Hello, world! Lots and lots and ... of text"
  [2]=>
  string(52) "2 test 3 Hello, world! Lots and lots and ... of text"
  [3]=>
  string(52) "3 test 3 Hello, world! Lots and lots and ... of text"
  [4]=>
  string(52) "4 test 3 Hello, world! Lots and lots and ... of text"
  [5]=>
  string(52) "5 test 3 Hello, world! Lots and lots and ... of text"
  [6]=>
  string(52) "6 test 3 Hello, world! Lots and lots and ... of text"
  [7]=>
  string(52) "7 test 3 Hello, world! Lots and lots and ... of text"
  [8]=>
  string(52) "8 test 3 Hello, world! Lots and lots and ... of text"
  [9]=>
  string(52) "9 test 3 Hello, world! Lots and lots and ... of text"
  [10]=>
  string(53) "10 test 3 Hello, world! Lots and lots and ... of text"
  [11]=>
  string(53) "11 test 3 Hello, world! Lots and lots and ... of text"
  [12]=>
  string(53) "12 test 3 Hello, world! Lots and lots and ... of text"
  [13]=>
  string(53) "13 test 3 Hello, world! Lots and lots and ... of text"
  [14]=>
  string(53) "14 test 3 Hello, world! Lots and lots and ... of text"
  [15]=>
  string(53) "15 test 3 Hello, world! Lots and lots and ... of text"
  [16]=>
  string(53) "16 test 3 Hello, world! Lots and lots and ... of text"
  [17]=>
  string(53) "17 test 3 Hello, world! Lots and lots and ... of text"
  [18]=>
  string(53) "18 test 3 Hello, world! Lots and lots and ... of text"
  [19]=>
  string(53) "19 test 3 Hello, world! Lots and lots and ... of text"
  [20]=>
  string(53) "20 test 3 Hello, world! Lots and lots and ... of text"
  [21]=>
  string(53) "21 test 3 Hello, world! Lots and lots and ... of text"
  [22]=>
  string(53) "22 test 3 Hello, world! Lots and lots and ... of text"
  [23]=>
  string(53) "23 test 3 Hello, world! Lots and lots and ... of text"
  [24]=>
  string(53) "24 test 3 Hello, world! Lots and lots and ... of text"
  [25]=>
  string(53) "25 test 3 Hello, world! Lots and lots and ... of text"
  [26]=>
  string(53) "26 test 3 Hello, world! Lots and lots and ... of text"
  [27]=>
  string(53) "27 test 3 Hello, world! Lots and lots and ... of text"
  [28]=>
  string(53) "28 test 3 Hello, world! Lots and lots and ... of text"
  [29]=>
  string(53) "29 test 3 Hello, world! Lots and lots and ... of text"
  [30]=>
  string(53) "30 test 3 Hello, world! Lots and lots and ... of text"
  [31]=>
  string(53) "31 test 3 Hello, world! Lots and lots and ... of text"
  [32]=>
  string(53) "32 test 3 Hello, world! Lots and lots and ... of text"
  [33]=>
  string(53) "33 test 3 Hello, world! Lots and lots and ... of text"
  [34]=>
  string(53) "34 test 3 Hello, world! Lots and lots and ... of text"
  [35]=>
  string(53) "35 test 3 Hello, world! Lots and lots and ... of text"
  [36]=>
  string(53) "36 test 3 Hello, world! Lots and lots and ... of text"
  [37]=>
  string(53) "37 test 3 Hello, world! Lots and lots and ... of text"
  [38]=>
  string(53) "38 test 3 Hello, world! Lots and lots and ... of text"
  [39]=>
  string(53) "39 test 3 Hello, world! Lots and lots and ... of text"
  [40]=>
  string(53) "40 test 3 Hello, world! Lots and lots and ... of text"
  [41]=>
  string(53) "41 test 3 Hello, world! Lots and lots and ... of text"
  [42]=>
  string(53) "42 test 3 Hello, world! Lots and lots and ... of text"
  [43]=>
  string(53) "43 test 3 Hello, world! Lots and lots and ... of text"
  [44]=>
  string(53) "44 test 3 Hello, world! Lots and lots and ... of text"
  [45]=>
  string(53) "45 test 3 Hello, world! Lots and lots and ... of text"
  [46]=>
  string(53) "46 test 3 Hello, world! Lots and lots and ... of text"
  [47]=>
  string(53) "47 test 3 Hello, world! Lots and lots and ... of text"
  [48]=>
  string(53) "48 test 3 Hello, world! Lots and lots and ... of text"
  [49]=>
  string(53) "49 test 3 Hello, world! Lots and lots and ... of text"
  [50]=>
  string(53) "50 test 3 Hello, world! Lots and lots and ... of text"
  [51]=>
  string(53) "51 test 3 Hello, world! Lots and lots and ... of text"
  [52]=>
  string(53) "52 test 3 Hello, world! Lots and lots and ... of text"
  [53]=>
  string(53) "53 test 3 Hello, world! Lots and lots and ... of text"
  [54]=>
  string(53) "54 test 3 Hello, world! Lots and lots and ... of text"
  [55]=>
  string(53) "55 test 3 Hello, world! Lots and lots and ... of text"
  [56]=>
  string(53) "56 test 3 Hello, world! Lots and lots and ... of text"
  [57]=>
  string(53) "57 test 3 Hello, world! Lots and lots and ... of text"
  [58]=>
  string(53) "58 test 3 Hello, world! Lots and lots and ... of text"
  [59]=>
  string(53) "59 test 3 Hello, world! Lots and lots and ... of text"
  [60]=>
  string(53) "60 test 3 Hello, world! Lots and lots and ... of text"
  [61]=>
  string(53) "61 test 3 Hello, world! Lots and lots and ... of text"
  [62]=>
  string(53) "62 test 3 Hello, world! Lots and lots and ... of text"
  [63]=>
  string(53) "63 test 3 Hello, world! Lots and lots and ... of text"
  [64]=>
  string(53) "64 test 3 Hello, world! Lots and lots and ... of text"
  [65]=>
  string(53) "65 test 3 Hello, world! Lots and lots and ... of text"
  [66]=>
  string(53) "66 test 3 Hello, world! Lots and lots and ... of text"
  [67]=>
  string(53) "67 test 3 Hello, world! Lots and lots and ... of text"
  [68]=>
  string(53) "68 test 3 Hello, world! Lots and lots and ... of text"
  [69]=>
  string(53) "69 test 3 Hello, world! Lots and lots and ... of text"
  [70]=>
  string(53) "70 test 3 Hello, world! Lots and lots and ... of text"
  [71]=>
  string(53) "71 test 3 Hello, world! Lots and lots and ... of text"
  [72]=>
  string(53) "72 test 3 Hello, world! Lots and lots and ... of text"
  [73]=>
  string(53) "73 test 3 Hello, world! Lots and lots and ... of text"
  [74]=>
  string(53) "74 test 3 Hello, world! Lots and lots and ... of text"
  [75]=>
  string(53) "75 test 3 Hello, world! Lots and lots and ... of text"
  [76]=>
  string(53) "76 test 3 Hello, world! Lots and lots and ... of text"
  [77]=>
  string(53) "77 test 3 Hello, world! Lots and lots and ... of text"
  [78]=>
  string(53) "78 test 3 Hello, world! Lots and lots and ... of text"
  [79]=>
  string(53) "79 test 3 Hello, world! Lots and lots and ... of text"
  [80]=>
  string(53) "80 test 3 Hello, world! Lots and lots and ... of text"
  [81]=>
  string(53) "81 test 3 Hello, world! Lots and lots and ... of text"
  [82]=>
  string(53) "82 test 3 Hello, world! Lots and lots and ... of text"
  [83]=>
  string(53) "83 test 3 Hello, world! Lots and lots and ... of text"
  [84]=>
  string(53) "84 test 3 Hello, world! Lots and lots and ... of text"
  [85]=>
  string(53) "85 test 3 Hello, world! Lots and lots and ... of text"
  [86]=>
  string(53) "86 test 3 Hello, world! Lots and lots and ... of text"
  [87]=>
  string(53) "87 test 3 Hello, world! Lots and lots and ... of text"
  [88]=>
  string(53) "88 test 3 Hello, world! Lots and lots and ... of text"
  [89]=>
  string(53) "89 test 3 Hello, world! Lots and lots and ... of text"
  [90]=>
  string(53) "90 test 3 Hello, world! Lots and lots and ... of text"
  [91]=>
  string(53) "91 test 3 Hello, world! Lots and lots and ... of text"
  [92]=>
  string(53) "92 test 3 Hello, world! Lots and lots and ... of text"
  [93]=>
  string(53) "93 test 3 Hello, world! Lots and lots and ... of text"
  [94]=>
  string(53) "94 test 3 Hello, world! Lots and lots and ... of text"
  [95]=>
  string(53) "95 test 3 Hello, world! Lots and lots and ... of text"
  [96]=>
  string(53) "96 test 3 Hello, world! Lots and lots and ... of text"
  [97]=>
  string(53) "97 test 3 Hello, world! Lots and lots and ... of text"
  [98]=>
  string(53) "98 test 3 Hello, world! Lots and lots and ... of text"
  [99]=>
  string(53) "99 test 3 Hello, world! Lots and lots and ... of text"
}
Test 4
array(100) {
  [0]=>
  string(52) "0 test 4 Hello, world! Lots and lots and ... of text"
  [1]=>
  string(52) "1 test 4 Hello, world! Lots and lots and ... of text"
  [2]=>
  string(52) "2 test 4 Hello, world! Lots and lots and ... of text"
  [3]=>
  string(52) "3 test 4 Hello, world! Lots and lots and ... of text"
  [4]=>
  string(52) "4 test 4 Hello, world! Lots and lots and ... of text"
  [5]=>
  string(52) "5 test 4 Hello, world! Lots and lots and ... of text"
  [6]=>
  string(52) "6 test 4 Hello, world! Lots and lots and ... of text"
  [7]=>
  string(52) "7 test 4 Hello, world! Lots and lots and ... of text"
  [8]=>
  string(52) "8 test 4 Hello, world! Lots and lots and ... of text"
  [9]=>
  string(52) "9 test 4 Hello, world! Lots and lots and ... of text"
  [10]=>
  string(53) "10 test 4 Hello, world! Lots and lots and ... of text"
  [11]=>
  string(53) "11 test 4 Hello, world! Lots and lots and ... of text"
  [12]=>
  string(53) "12 test 4 Hello, world! Lots and lots and ... of text"
  [13]=>
  string(53) "13 test 4 Hello, world! Lots and lots and ... of text"
  [14]=>
  string(53) "14 test 4 Hello, world! Lots and lots and ... of text"
  [15]=>
  string(53) "15 test 4 Hello, world! Lots and lots and ... of text"
  [16]=>
  string(53) "16 test 4 Hello, world! Lots and lots and ... of text"
  [17]=>
  string(53) "17 test 4 Hello, world! Lots and lots and ... of text"
  [18]=>
  string(53) "18 test 4 Hello, world! Lots and lots and ... of text"
  [19]=>
  string(53) "19 test 4 Hello, world! Lots and lots and ... of text"
  [20]=>
  string(53) "20 test 4 Hello, world! Lots and lots and ... of text"
  [21]=>
  string(53) "21 test 4 Hello, world! Lots and lots and ... of text"
  [22]=>
  string(53) "22 test 4 Hello, world! Lots and lots and ... of text"
  [23]=>
  string(53) "23 test 4 Hello, world! Lots and lots and ... of text"
  [24]=>
  string(53) "24 test 4 Hello, world! Lots and lots and ... of text"
  [25]=>
  string(53) "25 test 4 Hello, world! Lots and lots and ... of text"
  [26]=>
  string(53) "26 test 4 Hello, world! Lots and lots and ... of text"
  [27]=>
  string(53) "27 test 4 Hello, world! Lots and lots and ... of text"
  [28]=>
  string(53) "28 test 4 Hello, world! Lots and lots and ... of text"
  [29]=>
  string(53) "29 test 4 Hello, world! Lots and lots and ... of text"
  [30]=>
  string(53) "30 test 4 Hello, world! Lots and lots and ... of text"
  [31]=>
  string(53) "31 test 4 Hello, world! Lots and lots and ... of text"
  [32]=>
  string(53) "32 test 4 Hello, world! Lots and lots and ... of text"
  [33]=>
  string(53) "33 test 4 Hello, world! Lots and lots and ... of text"
  [34]=>
  string(53) "34 test 4 Hello, world! Lots and lots and ... of text"
  [35]=>
  string(53) "35 test 4 Hello, world! Lots and lots and ... of text"
  [36]=>
  string(53) "36 test 4 Hello, world! Lots and lots and ... of text"
  [37]=>
  string(53) "37 test 4 Hello, world! Lots and lots and ... of text"
  [38]=>
  string(53) "38 test 4 Hello, world! Lots and lots and ... of text"
  [39]=>
  string(53) "39 test 4 Hello, world! Lots and lots and ... of text"
  [40]=>
  string(53) "40 test 4 Hello, world! Lots and lots and ... of text"
  [41]=>
  string(53) "41 test 4 Hello, world! Lots and lots and ... of text"
  [42]=>
  string(53) "42 test 4 Hello, world! Lots and lots and ... of text"
  [43]=>
  string(53) "43 test 4 Hello, world! Lots and lots and ... of text"
  [44]=>
  string(53) "44 test 4 Hello, world! Lots and lots and ... of text"
  [45]=>
  string(53) "45 test 4 Hello, world! Lots and lots and ... of text"
  [46]=>
  string(53) "46 test 4 Hello, world! Lots and lots and ... of text"
  [47]=>
  string(53) "47 test 4 Hello, world! Lots and lots and ... of text"
  [48]=>
  string(53) "48 test 4 Hello, world! Lots and lots and ... of text"
  [49]=>
  string(53) "49 test 4 Hello, world! Lots and lots and ... of text"
  [50]=>
  string(53) "50 test 4 Hello, world! Lots and lots and ... of text"
  [51]=>
  string(53) "51 test 4 Hello, world! Lots and lots and ... of text"
  [52]=>
  string(53) "52 test 4 Hello, world! Lots and lots and ... of text"
  [53]=>
  string(53) "53 test 4 Hello, world! Lots and lots and ... of text"
  [54]=>
  string(53) "54 test 4 Hello, world! Lots and lots and ... of text"
  [55]=>
  string(53) "55 test 4 Hello, world! Lots and lots and ... of text"
  [56]=>
  string(53) "56 test 4 Hello, world! Lots and lots and ... of text"
  [57]=>
  string(53) "57 test 4 Hello, world! Lots and lots and ... of text"
  [58]=>
  string(53) "58 test 4 Hello, world! Lots and lots and ... of text"
  [59]=>
  string(53) "59 test 4 Hello, world! Lots and lots and ... of text"
  [60]=>
  string(53) "60 test 4 Hello, world! Lots and lots and ... of text"
  [61]=>
  string(53) "61 test 4 Hello, world! Lots and lots and ... of text"
  [62]=>
  string(53) "62 test 4 Hello, world! Lots and lots and ... of text"
  [63]=>
  string(53) "63 test 4 Hello, world! Lots and lots and ... of text"
  [64]=>
  string(53) "64 test 4 Hello, world! Lots and lots and ... of text"
  [65]=>
  string(53) "65 test 4 Hello, world! Lots and lots and ... of text"
  [66]=>
  string(53) "66 test 4 Hello, world! Lots and lots and ... of text"
  [67]=>
  string(53) "67 test 4 Hello, world! Lots and lots and ... of text"
  [68]=>
  string(53) "68 test 4 Hello, world! Lots and lots and ... of text"
  [69]=>
  string(53) "69 test 4 Hello, world! Lots and lots and ... of text"
  [70]=>
  string(53) "70 test 4 Hello, world! Lots and lots and ... of text"
  [71]=>
  string(53) "71 test 4 Hello, world! Lots and lots and ... of text"
  [72]=>
  string(53) "72 test 4 Hello, world! Lots and lots and ... of text"
  [73]=>
  string(53) "73 test 4 Hello, world! Lots and lots and ... of text"
  [74]=>
  string(53) "74 test 4 Hello, world! Lots and lots and ... of text"
  [75]=>
  string(53) "75 test 4 Hello, world! Lots and lots and ... of text"
  [76]=>
  string(53) "76 test 4 Hello, world! Lots and lots and ... of text"
  [77]=>
  string(53) "77 test 4 Hello, world! Lots and lots and ... of text"
  [78]=>
  string(53) "78 test 4 Hello, world! Lots and lots and ... of text"
  [79]=>
  string(53) "79 test 4 Hello, world! Lots and lots and ... of text"
  [80]=>
  string(53) "80 test 4 Hello, world! Lots and lots and ... of text"
  [81]=>
  string(53) "81 test 4 Hello, world! Lots and lots and ... of text"
  [82]=>
  string(53) "82 test 4 Hello, world! Lots and lots and ... of text"
  [83]=>
  string(53) "83 test 4 Hello, world! Lots and lots and ... of text"
  [84]=>
  string(53) "84 test 4 Hello, world! Lots and lots and ... of text"
  [85]=>
  string(53) "85 test 4 Hello, world! Lots and lots and ... of text"
  [86]=>
  string(53) "86 test 4 Hello, world! Lots and lots and ... of text"
  [87]=>
  string(53) "87 test 4 Hello, world! Lots and lots and ... of text"
  [88]=>
  string(53) "88 test 4 Hello, world! Lots and lots and ... of text"
  [89]=>
  string(53) "89 test 4 Hello, world! Lots and lots and ... of text"
  [90]=>
  string(53) "90 test 4 Hello, world! Lots and lots and ... of text"
  [91]=>
  string(53) "91 test 4 Hello, world! Lots and lots and ... of text"
  [92]=>
  string(53) "92 test 4 Hello, world! Lots and lots and ... of text"
  [93]=>
  string(53) "93 test 4 Hello, world! Lots and lots and ... of text"
  [94]=>
  string(53) "94 test 4 Hello, world! Lots and lots and ... of text"
  [95]=>
  string(53) "95 test 4 Hello, world! Lots and lots and ... of text"
  [96]=>
  string(53) "96 test 4 Hello, world! Lots and lots and ... of text"
  [97]=>
  string(53) "97 test 4 Hello, world! Lots and lots and ... of text"
  [98]=>
  string(53) "98 test 4 Hello, world! Lots and lots and ... of text"
  [99]=>
  string(53) "99 test 4 Hello, world! Lots and lots and ... of text"
}
