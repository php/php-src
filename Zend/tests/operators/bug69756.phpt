--TEST--
Fixed bug #69756 (Fatal error: Nesting level too deep - recursive dependency? with ===).
--FILE--
<?php
$arr = range(1, 2);
foreach($arr as &$item ) {
    var_dump($arr === array(1, 2));
}
?>
--EXPECT--
bool(true)
bool(true)
