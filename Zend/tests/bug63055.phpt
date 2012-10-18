--TEST--
Bug #63055 (Segfault in zend_gc with SF2 testsuite)
--FILE--
<?php
for ($i=0; $i<9998; $i++) {
    $array = array();
    $array[0] = &$array;
    unset($array);
}

$matches = array("foo" => "bar");
$dummy   = array("dummy");
$dummy[1] = &$dummy;

$matches[1] = &$matches;
$matches[2] = $dummy;

$ma         = $matches;
preg_match_all("/(\d)+/", "foo123456bar", $matches);
echo "okey";
?>
--EXPECTF--
okey
