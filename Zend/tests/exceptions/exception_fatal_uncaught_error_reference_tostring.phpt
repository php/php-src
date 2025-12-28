--TEST--
Exception fatal uncaught error with reference __toString
--FILE--
<?php

class MyException extends Exception {
    private $field = 'my string';
    public function &__toString(): string {
        return $this->field;
    }
}

// Must not be caught to trigger the issue!
throw new MyException;

?>
--EXPECTF--
Fatal error: Uncaught my string
  thrown in %s on line %d
