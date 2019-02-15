--TEST--
PDO OCI: Fetches 10K records from a table that contains 1 number and 2 LOB columns (stress test)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

// !! Note: uses data inserted in pdo_oci_stream_2a.phpt !!

require('ext/pdo/tests/pdo_test.inc');
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);  // Let's use streams

// Since each column only has one lob descriptor, the last row is
// shown twice because the lob descriptor for each column is reused in
// the stream

$i = 0;
$j = 9;
$a_val = ord('a');
foreach($db->query("select data1 as d4_1, data2 as d4_2 from pdo_oci_stream_2 order by id") as $row) {
    $a = $row['d4_1'];
	$a1 = $row['d4_2'];

    $str1 = stream_get_contents($a);
	$str2 = stream_get_contents($a1);

    $str1len = strlen($str1);
	$str2len = strlen($str2);

    $b = ord($str1[0]);
	$b1 = ord($str2[0]);

    if (($b != ($a_val + $i)) && ($str1len != (4086 + $i)) &&
        ($b1 != ($a_val + $j)) && ($str2len != (4086 + $j))) {
        printf("There is a bug!\n");
        printf("Col1:\n");
        printf("a_val = %d\n", $a_val);
        printf("b     = %d\n", $b);
        printf("i     = %d\n", $i);
        printf("str1len = %d\n", $str1len);

        printf("Col2:\n");
        printf("a_val = %d\n", $a_val);
        printf("b1    = %d\n", $b1);
        printf("j     = %d\n", $j);
        printf("str2len = %d\n", $str1len);

    }
    $i++;
    if ($i>9)
        $i = 0;
	$j--;
	if ($j<0)
        $j = 9;
}
echo "Fetch operation done!\n";

/* Cleanup */
$db->exec("drop table pdo_oci_stream_2");

?>
--EXPECT--
Fetch operation done!
