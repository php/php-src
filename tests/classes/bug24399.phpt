--TEST--
Bug #24399: is_subclass_of() crashes when parent class doesn't exist.
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class dooh {
    var $blah;
}
$d = new dooh;
var_dump(is_subclass_of($d, 'dooh'));
?>
--EXPECT--
bool(false)
