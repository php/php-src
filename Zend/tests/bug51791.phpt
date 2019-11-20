--TEST--
Bug #51791 (constant() failed to check undefined constant and php interpreter stopped)
--FILE--
<?php

class A  {
    const B = 1;
}

try {
    constant('A::B1');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Couldn't find constant A::B1
