--TEST--
Bug #71695 (Global variables are reserved before execution)
--FILE--
<?php
function provideGlobals() {
    var_dump(array_key_exists("foo", $GLOBALS));
    var_dump(isset($GLOBALS["foo"]));
    $GLOBALS += array("foo" => "foo");
}

provideGlobals();
echo $foo;
?>
--EXPECT--
bool(false)
bool(false)
foo
