--TEST--
Bug #55082: var_export() doesn't escape properties properly
--FILE--
<?php
    $x = new stdClass();
    $x->{'\'\\'} = 7;
    echo var_export($x);
?>
--EXPECT--
(object) array(
   '\'\\' => 7,
)
