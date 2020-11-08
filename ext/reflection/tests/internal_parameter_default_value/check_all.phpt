--TEST--
Check that all internal parameter defaults evaluate without error
--FILE--
<?php

function checkDefaults(ReflectionFunctionAbstract $rf) {
    foreach ($rf->getParameters() as $param) {
        if ($param->isDefaultValueAvailable()) {
            try {
                $param->getDefaultValue();
            } catch (Error $e) {
                echo "{$rf->getName()}: {$e->getMessage()}\n";
            }
        }
    }
}

foreach (get_defined_functions()["internal"] as $func) {
    $rf = new ReflectionFunction($func);
    checkDefaults($rf);
}

foreach (get_declared_classes() as $class) {
    $rc = new ReflectionClass($class);
    foreach ($rc->getMethods() as $method) {
        checkDefaults($method);
    }
}

?>
===DONE===
--EXPECT--
===DONE===
