--TEST--
__serialize() mechanism (004): Delayed __unserialize() calls
--FILE--
<?php

class Wakeup {
    public $data;
    public function __construct(array $data) {
        $this->data = $data;
    }
    public function __wakeup() {
        echo "__wakeup() called\n";
        var_dump($this->data);
        $this->woken_up = true;
    }
}

class Unserialize {
    public $data;
    public function __construct(array $data) {
        $this->data = $data;
    }
    public function __serialize() {
        return $this->data;
    }
    public function __unserialize(array $data) {
        $this->data = $data;
        echo "__unserialize() called\n";
        var_dump($this->data);
        $this->unserialized = true;
    }
}

$obj = new Wakeup([new Unserialize([new Wakeup([new Unserialize([])])])]);
var_dump($s = serialize($obj));
var_dump(unserialize($s));

?>
--EXPECT--
string(126) "O:6:"Wakeup":1:{s:4:"data";a:1:{i:0;O:11:"Unserialize":1:{i:0;O:6:"Wakeup":1:{s:4:"data";a:1:{i:0;O:11:"Unserialize":0:{}}}}}}"
__unserialize() called
array(0) {
}
__wakeup() called
array(1) {
  [0]=>
  object(Unserialize)#8 (2) {
    ["data"]=>
    array(0) {
    }
    ["unserialized"]=>
    bool(true)
  }
}
__unserialize() called
array(1) {
  [0]=>
  object(Wakeup)#7 (2) {
    ["data"]=>
    array(1) {
      [0]=>
      object(Unserialize)#8 (2) {
        ["data"]=>
        array(0) {
        }
        ["unserialized"]=>
        bool(true)
      }
    }
    ["woken_up"]=>
    bool(true)
  }
}
__wakeup() called
array(1) {
  [0]=>
  object(Unserialize)#6 (2) {
    ["data"]=>
    array(1) {
      [0]=>
      object(Wakeup)#7 (2) {
        ["data"]=>
        array(1) {
          [0]=>
          object(Unserialize)#8 (2) {
            ["data"]=>
            array(0) {
            }
            ["unserialized"]=>
            bool(true)
          }
        }
        ["woken_up"]=>
        bool(true)
      }
    }
    ["unserialized"]=>
    bool(true)
  }
}
object(Wakeup)#5 (2) {
  ["data"]=>
  array(1) {
    [0]=>
    object(Unserialize)#6 (2) {
      ["data"]=>
      array(1) {
        [0]=>
        object(Wakeup)#7 (2) {
          ["data"]=>
          array(1) {
            [0]=>
            object(Unserialize)#8 (2) {
              ["data"]=>
              array(0) {
              }
              ["unserialized"]=>
              bool(true)
            }
          }
          ["woken_up"]=>
          bool(true)
        }
      }
      ["unserialized"]=>
      bool(true)
    }
  }
  ["woken_up"]=>
  bool(true)
}
