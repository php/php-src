--TEST--
GH-16792 (Assertion failure when an error handler mutates $GLOBALS during a by-ref sort)
--FILE--
<?php
set_error_handler(function() {
    $GLOBALS['x'] = $GLOBALS['y'];
});
function x(&$s) {
    $s[0] = 1;
}
x($y);
arsort($y, $fusion);
echo "ok\n";
?>
--EXPECT--
ok
