<?php

/* assumes that you have the freetds db.lib on windows */

dl('php_pdo.dll');
dl('php_pdo_sybase.dll');

try {

$db = new PDO('sybase:', 'pdo', 'pdo');
$db->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_EXCEPTION);
debug_zval_dump($db);

$stmt = $db->prepare('select 10');
debug_zval_dump($stmt);

$x = $stmt->execute();
debug_zval_dump($x);

while (($r = $stmt->fetch())) {
	print_r($r);
}

} catch (Exception $e) {
	print $e;
}

$stmt = null;
$db = null;
echo "All done\n";
?>
