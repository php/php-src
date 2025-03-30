--TEST--
usage in an interface
--FILE--
<?php

interface Outer {
    protected class Inner {}
}

?>
--EXPECTF--
Fatal error: Cannot declare Outer::$Inner as protected in interface Outer in %s on line %d
