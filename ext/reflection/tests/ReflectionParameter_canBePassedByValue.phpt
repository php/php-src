--TEST--
ReflectionParameter class - canBePassedByValue() method.
--FILE--
<?php

function aux($fun) {

    $func = new ReflectionFunction($fun);
    $parameters = $func->getParameters();
    foreach($parameters as $parameter) {
        echo "Name: ", $parameter->getName(), "\n";
        echo "Is passed by reference: ", $parameter->isPassedByReference()?"yes":"no", "\n";
        echo "Can be passed by value: ", $parameter->canBePassedByValue()?"yes":"no", "\n";
        echo "\n";
    }

}

echo "=> array_multisort:\n\n";

aux('array_multisort');


echo "=> sort:\n\n";

aux('sort');

echo "=> user function:\n\n";

function ufunc(&$array1, $array2) {}

aux('ufunc');

echo "Done.\n";

?>
--EXPECT--
=> array_multisort:

Name: array
Is passed by reference: yes
Can be passed by value: yes

Name: rest
Is passed by reference: yes
Can be passed by value: yes

=> sort:

Name: array
Is passed by reference: yes
Can be passed by value: no

Name: flags
Is passed by reference: no
Can be passed by value: yes

=> user function:

Name: array1
Is passed by reference: yes
Can be passed by value: no

Name: array2
Is passed by reference: no
Can be passed by value: yes

Done.
