<?php

/* assumes that you have the freetds db.lib on windows */

dl('php_pdo.dll');
dl('php_pdo_sybase.dll');

$db = new PDO('sybase:', 'pdo', 'pdo');
debug_zval_dump($db);

$stmt = $db->prepare('sp_helpdb');
debug_zval_dump($stmt);

$x = $stmt->execute();
debug_zval_dump($x);

while (($r = $stmt->fetch())) {
	print_r($r);
}

$stmt = null;
$db = null;
echo "All done\n";
?>
