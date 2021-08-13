--TEST--
Bug #41917 (invalid scale and precision)
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__."/connect.inc");

$s = oci_parse($c, 'drop table field_funcs2_tab');
@oci_execute($s);

$t = array("C01" => "NUMBER",
           "C02" => "NUMBER(*,1)",
           "C03" => "NUMBER(9)",
           "C04" => "NUMBER(9,2)",
           "C05" => "NUMBER(9,1)",
           "C06" => "NUMBER(7,-2)",
           "C07" => "DECIMAL(4,9)",
           "C08" => "NUMERIC(4,9)",
           "C09" => "DECIMAL(4)",
           "C10" => "INTEGER",
           "C11" => "INT",
           "C12" => "SMALLINT",
           "C13" => "FLOAT",
           "C14" => "FLOAT(9)",
           "C15" => "DOUBLE PRECISION",
           "C16" => "REAL",
           );

$stmt = "create table field_funcs2_tab (\n";
foreach ($t as $colname => $type) {
    $stmt .= "$colname $type,\n";
}
$stmt[strlen($stmt)-2] = ")";

$s = oci_parse($c, $stmt);
oci_execute($s);

$s = oci_parse($c, "select * from field_funcs2_tab");
oci_execute($s);

for ($i = 1; $i <= oci_num_fields($s); $i++) {
    $name = oci_field_name($s, $i);
    $precision = oci_field_precision($s, $i);
    $scale = oci_field_scale($s, $i);
    echo "$name ".$t[$name] .": precision $precision, scale $scale\n";
}

echo "Done\n";
?>
--EXPECT--
C01 NUMBER: precision 0, scale -127
C02 NUMBER(*,1): precision 38, scale 1
C03 NUMBER(9): precision 9, scale 0
C04 NUMBER(9,2): precision 9, scale 2
C05 NUMBER(9,1): precision 9, scale 1
C06 NUMBER(7,-2): precision 7, scale -2
C07 DECIMAL(4,9): precision 4, scale 9
C08 NUMERIC(4,9): precision 4, scale 9
C09 DECIMAL(4): precision 4, scale 0
C10 INTEGER: precision 38, scale 0
C11 INT: precision 38, scale 0
C12 SMALLINT: precision 38, scale 0
C13 FLOAT: precision 126, scale -127
C14 FLOAT(9): precision 9, scale -127
C15 DOUBLE PRECISION: precision 126, scale -127
C16 REAL: precision 63, scale -127
Done
