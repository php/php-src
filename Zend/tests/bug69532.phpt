--TEST--
Bug #69532: array_multisort is chocking when using it's own constants
--FILE--
<?php

namespace Foo;

$origins = array();
$profiles = array();
$all_files = array();

array_multisort($origins, SORT_ASC, $profiles, SORT_ASC, $all_files);

?>
===DONE===
--EXPECT--
===DONE===
