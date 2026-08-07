--TEST--
Pipe assign operator error: cannot reassign $this
--FILE--
<?php

class Bar {
    public function test() {
        $this |>= trim(...);
    }
}

?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %s on line %d
