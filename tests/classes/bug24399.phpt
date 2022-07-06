--TEST--
Bug #24399 (is_subclass_of() crashes when parent class doesn't exist)
--FILE--
<?php
class dooh {
    public $blah;
}
$d = new dooh;
var_dump(is_subclass_of($d, 'dooh'));
?>
--EXPECT--
bool(false)
