--TEST--
Check that SplObjectStorage::removeAllExcept generate a warning and returns NULL when passed non-object param
--CREDITS--
Matthew Turland (me@matthewturland.com)
Based on work done at PHPNW Testfest 2009 by Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$data_provider = array(
   array(),
   true,
   "string",
   12345,
   1.2345,
   NULL
);

foreach($data_provider as $input) {

   $s = new SplObjectStorage();

   var_dump($s->removeAllExcept($input));
}

?>
--EXPECTF--
Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, array given in %s on line %d
NULL

Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, boolean given in %s on line %d
NULL

Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, %unicode_string_optional% given in %s on line %d
NULL

Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, integer given in %s on line %d
NULL

Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, float given in %s on line %d
NULL

Warning: SplObjectStorage::removeAllExcept() expects parameter 1 to be SplObjectStorage, null given in %s on line %d
NULL

