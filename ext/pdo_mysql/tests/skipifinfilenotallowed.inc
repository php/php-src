<?php
$db = MySQLPDOTest::factory();
$stmt = $db->query("SHOW VARIABLES LIKE 'local_infile'");
if (($row = $stmt->fetch(PDO::FETCH_ASSOC)) && ($row['value'] != 'ON'))
	die("skip Server variable 'local_infile' seems not set to 'ON', found '". $row['value'] ."'");
?>
