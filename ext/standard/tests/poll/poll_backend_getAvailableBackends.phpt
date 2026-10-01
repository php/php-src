--TEST--
\Io\Poll\Backend::getAvailableBackends() - loop
--FILE--
<?php
$objs = [];
foreach (($expr1 = \Io\Poll\Backend::getAvailableBackends()) as $expr2) {
    $objs[] = $expr2;
}
echo "Okay\n";
?>
--EXPECT--
Okay
