--TEST--
GH-14712: segfault on PDORow
--EXTENSIONS--
pdo_sqlite
--CREDITS--
YuanchengJiang
--FILE--
<?php
$db = new PDO('sqlite::memory:');

try {
	$db->query("select 1 as queryStringxx")->fetch(PDO::FETCH_LAZY)->documentElement->firstChild->nextElementSibling->textContent = "Ã©";
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Attempt to modify property "firstChild" on null
