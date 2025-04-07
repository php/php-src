--TEST--
usage in an interface
--FILE--
<?php

interface Outer {
    private class Inner {}
}

?>
--EXPECTF--
Fatal error: Cannot declare Outer::$Inner as private in interface Outer in %s on line %d
