--TEST--
Exception with delayed message computation
--FILE--
<?php

class MyException extends Exception {
    public $message;
    public $messageCallback;

    public function __construct() {
        $this->messageCallback = static function() {
            return "Foobar";
        };
        $this->message = new class($this->message, $this->messageCallback) {
            private $message;
            private $messageCallback;

            public function __construct(&$message, &$messageCallback)
            {
                $this->message = &$message;
                $this->messageCallback = &$messageCallback;
            }

            public function __toString(): string
            {
                $messageCallback = $this->messageCallback;
                $this->messageCallback = null;
                return $this->message = $messageCallback();
            }
        };
    }
}

throw new MyException;

?>
--EXPECTF--
Fatal error: Uncaught MyException: Foobar in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
