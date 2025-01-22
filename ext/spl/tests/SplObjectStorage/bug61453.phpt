--TEST--
Bug #61453:	SplObjectStorage does not identify objects correctly
--FILE--
<?php
$limit = 1000;
$objects = new SplObjectStorage;
for($i = 0; $i < $limit; $i++){
    $object = new StdClass;

    if(isset($objects[$object])){
        die("this should never happen, but did after $i iteration");
    }

    $objects[$object] = 1;
}
?>
==DONE==
--EXPECT--
==DONE==
