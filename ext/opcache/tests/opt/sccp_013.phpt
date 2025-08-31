--TEST--
SCCP 013: Conditional Constant Propagation of non-escaping array elements on PHI
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function loadEntities($entity_information) {
    $entity_types = [];
    foreach ($entity_information as $info) {
        $entity_types[$info] = 1;
    }
    var_dump((bool)($entity_types[$info]));
}

loadEntities(array("first", "second"));
?>
--EXPECT--
bool(true)
