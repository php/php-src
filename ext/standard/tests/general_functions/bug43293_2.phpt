--TEST--
Bug#43293 (Multiple segfaults in getopt())
--INI--
register_argc_argv=Off
--FILE--
<?php
$argv = array(true, false);
var_dump(getopt("abcd"));
?>
--EXPECT--
array(0) {
}

