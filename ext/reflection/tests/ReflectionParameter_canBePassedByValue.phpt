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

function ufunc(&$arg1, $arg2) {}

aux('ufunc');

echo "Done.\n";

?>
--EXPECT--
=> array_multisort:

Name: arr1
Is passed by reference: yes
Can be passed by value: yes

Name: sort_order
Is passed by reference: yes
Can be passed by value: yes

Name: sort_flags
Is passed by reference: yes
Can be passed by value: yes

Name: arr2
Is passed by reference: yes
Can be passed by value: yes

=> sort:

Name: arg
Is passed by reference: yes
Can be passed by value: no

Name: sort_flags
Is passed by reference: no
Can be passed by value: yes

=> user function:

Name: arg1
Is passed by reference: yes
Can be passed by value: no

Name: arg2
Is passed by reference: no
Can be passed by value: yes

Done.
