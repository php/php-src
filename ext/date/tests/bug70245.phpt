--TEST--
Bug #70245 (strtotime does not emit warning when 2nd parameter is object or string)
--FILE--
<?php
$d = new DateTime('2011-01-15 00:00:00');
try {
    var_dump(strtotime('-1 month', $d));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
strtotime(): Argument #2 ($baseTimestamp) must be of type ?int, DateTime given
