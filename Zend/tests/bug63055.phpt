--TEST--
Bug #63055 (Segfault in zend_gc with SF2 testsuite)
--INI--
zend.enable_gc=1
--FILE--
<?php
/* the default gc root size is 10,000 */
for ($i=0; $i<9998; $i++) {
    $array = array();
    $array[0] = &$array;
    unset($array);
}

$matches = array("foo" => "bar"); /* this bucket will trigger the segfault */
$dummy   = array("dummy");        /* used to trigger gc_collect_cycles */
$dummy[1] = &$dummy;

$matches[1] = &$matches;
$matches[2] = $dummy;

str_replace("foo", "bar", "foobar", $matches);
echo "okey";
?>
--EXPECTF--
okey
