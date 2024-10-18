--TEST--
ZE2 A method cannot be redeclared abstract
--FILE--
<?php

class pass {
    function show() {
        echo "Call to function show()\n";
    }
}

class fail extends pass {
    abstract function show();
}

echo "Done\n"; // Shouldn't be displayed
?>
--EXPECTF--
Fatal error: Class fail contains abstract method fail::show and must therefore be declared abstract in %sabstract_redeclare.php on line %d
