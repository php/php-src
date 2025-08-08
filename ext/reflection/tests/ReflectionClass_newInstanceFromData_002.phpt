--TEST--
ReflectionClass::newInstanceFromData - internal class
--FILE--
<?php

$rcDateTime = new ReflectionClass('DateTime');
$rcPDOStatement = new ReflectionClass('PDOStatement');

try
{
    $rcDateTime->newInstanceFromData([], ['now', new DateTimeZone('UTC')]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    $rcPDOStatement->newInstanceFromData(['a' => 123]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Exception: Class DateTime is an internal class that cannot be instantiated from data
Exception: Class PDOStatement is an internal class that cannot be instantiated from data
