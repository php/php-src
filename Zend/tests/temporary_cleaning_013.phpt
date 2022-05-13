--TEST--
Exceptions thrown in operand cleaning must cause leak of return value
--FILE--
<?php

try {
    var_dump(new class {
        function __toString() { return "a"; }
        function __destruct() { throw new Exception; }
    } . "foo");
} catch (Exception $e) { print "caught Exception 1\n"; }

try {
    var_dump([0] + [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 2\n"; }

try {
    $foo = [0];
    var_dump($foo += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 3\n"; }

try {
    $foo = (object)["foo" => [0]];
    var_dump($foo->foo += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 4\n"; }

try {
    $foo = new class {
        function __get($x) { return [0]; }
        function __set($x, $y) {}
    };
    var_dump($foo->foo += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 5\n"; }

try {
    $foo = new class {
        public $bar = [0];
        function &__get($x) { return $this->bar; }
    };
    var_dump($foo->foo += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 6\n"; }

try {
    $foo = new class implements ArrayAccess {
        function offsetGet($x): mixed { return [0]; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
    };
    var_dump($foo[0] += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 7\n"; }

try {
    $foo = new class implements ArrayAccess {
        public $foo = [0];
        function &offsetGet($x): bool { return $this->foo; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
    };
    var_dump($foo[0] += [new class {
        function __destruct() { throw new Exception; }
    }]);
} catch (Exception $e) { print "caught Exception 8\n"; }

try {
    var_dump((function() { return new class {
        public $foo;
        function __construct() { $this->foo = new stdClass; }
        function __destruct() { throw new Exception; }
    }; })()->foo++);
} catch (Exception $e) { print "caught Exception 9\n"; }

try {
    var_dump((function() { return new class {
        function __get($x) { return new stdClass; }
        function __set($x, $y) {}
        function __destruct() { throw new Exception; }
    }; })()->foo++);
} catch (Exception $e) { print "caught Exception 10\n"; }

try {
    var_dump((function() { return new class {
        public $bar;
        function __construct() { $this->bar = new stdClass; }
        function &__get($x) { return $this->bar; }
        function __destruct() { throw new Exception; }
    }; })()->foo++);
} catch (Exception $e) { print "caught Exception 11\n"; }

try {
    var_dump(++(function() { return new class {
        function __construct() { $this->foo = new stdClass; }
        function __destruct() { throw new Exception; }
    }; })()->foo);
} catch (Exception $e) { print "caught Exception 12\n"; }

try {
    var_dump(++(function() { return new class {
        function __get($x) { return new stdClass; }
        function __set($x, $y) {}
        function __destruct() { throw new Exception; }
    }; })()->foo);
} catch (Exception $e) { print "caught Exception 13\n"; }

try {
    var_dump(++(function() { return new class {
        public $bar;
        function __construct() { $this->bar = new stdClass; }
        function &__get($x) { return $this->bar; }
        function __destruct() { throw new Exception; }
    }; })()->foo);
} catch (Exception $e) { print "caught Exception 14\n"; }

try {
    var_dump((function() { return new class implements ArrayAccess {
        function offsetGet($x): mixed { return [new stdClass]; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
        function __destruct() { throw new Exception; }
    }; })()[0]++);
} catch (Exception $e) { print "caught Exception 15\n"; }

try {
    var_dump(++(function() { return new class implements ArrayAccess {
        function offsetGet($x): mixed { return [new stdClass]; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
        function __destruct() { throw new Exception; }
    }; })()[0]);
} catch (Exception $e) { print "caught Exception 16\n"; }

try {
    var_dump((new class {
        public $foo;
        function __construct() { $this->foo = new stdClass; }
        function __destruct() { throw new Exception; }
    })->foo);
} catch (Exception $e) { print "caught Exception 17\n"; }

try {
    var_dump((new class {
        function __get($x) { return new stdClass; }
        function __set($x, $y) {}
        function __destruct() { throw new Exception; }
    })->foo);
} catch (Exception $e) { print "caught Exception 18\n"; }

try {
    var_dump((new class implements ArrayAccess {
        function offsetGet($x): mixed { return [new stdClass]; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
        function __destruct() { throw new Exception; }
    })[0]);
} catch (Exception $e) { print "caught Exception 19\n"; }

try {
    var_dump(isset((new class {
        public $foo;
        function __construct() { $this->foo = new stdClass; }
        function __destruct() { throw new Exception; }
    })->foo->bar));
} catch (Exception $e) { print "caught Exception 20\n"; }

try {
    var_dump(isset((new class {
        function __get($x) { return new stdClass; }
        function __set($x, $y) {}
        function __destruct() { throw new Exception; }
    })->foo->bar));
} catch (Exception $e) { print "caught Exception 21\n"; }

try {
    var_dump(isset((new class implements ArrayAccess {
        function offsetGet($x): mixed { return [new stdClass]; }
        function offsetSet($x, $y): void {}
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
        function __destruct() { throw new Exception; }
    })[0]->bar));
} catch (Exception $e) { print "caught Exception 22\n"; }

try {
    $foo = new class {
        function __destruct() { throw new Exception; }
    };
    var_dump($foo = new stdClass);
} catch (Exception $e) { print "caught Exception 23\n"; }

try {
    $foo = [new class {
        function __destruct() { throw new Exception; }
    }];
    var_dump($foo[0] = new stdClass);
} catch (Exception $e) { print "caught Exception 24\n"; }

try {
    $foo = (object) ["foo" => new class {
        function __destruct() { throw new Exception; }
    }];
    var_dump($foo->foo = new stdClass);
} catch (Exception $e) { print "caught Exception 25\n"; }

try {
    $foo = new class {
        function __get($x) {}
        function __set($x, $y) { throw new Exception; }
    };
    var_dump($foo->foo = new stdClass);
} catch (Exception $e) { print "caught Exception 26\n"; }

try {
    $foo = new class implements ArrayAccess {
        function offsetGet($x): mixed {}
        function offsetSet($x, $y): void { throw new Exception; }
        function offsetExists($x): bool { return true; }
        function offsetUnset($x): void {}
    };
    var_dump($foo[0] = new stdClass);
} catch (Exception $e) { print "caught Exception 27\n"; }

try {
    $foo = new class {
        function __destruct() { throw new Exception; }
    };
    $bar = new stdClass;
    var_dump($foo = &$bar);
} catch (Exception $e) { print "caught Exception 28\n"; }

try {
    $f = function() {
        return new class {
            function __toString() { return "a"; }
            function __destruct() { throw new Exception; }
        };
    };
    var_dump("{$f()}foo");
} catch (Exception $e) { print "caught Exception 29\n"; }

try {
    $f = function() {
        return new class {
            function __toString() { return "a"; }
            function __destruct() { throw new Exception; }
        };
    };
    var_dump("bar{$f()}foo");
} catch (Exception $e) { print "caught Exception 30\n"; }

try {
    var_dump((string) new class {
        function __toString() { $x = "Z"; return ++$x; }
        function __destruct() { throw new Exception; }
    });
} catch (Exception $e) { print "caught Exception 31\n"; }

try {
    var_dump(clone (new class {
        function __clone() { throw new Exception; }
    }));
} catch (Exception $e) { print "caught Exception 32\n"; }

?>
--EXPECTF--
caught Exception 1
caught Exception 2
caught Exception 3
caught Exception 4
caught Exception 5

Deprecated: Creation of dynamic property class@anonymous::$foo is deprecated in %s on line %d
caught Exception 6
caught Exception 7
caught Exception 8
caught Exception 9
caught Exception 10

Deprecated: Creation of dynamic property class@anonymous::$foo is deprecated in %s on line %d
caught Exception 11

Deprecated: Creation of dynamic property class@anonymous::$foo is deprecated in %s on line %d
caught Exception 12
caught Exception 13

Deprecated: Creation of dynamic property class@anonymous::$foo is deprecated in %s on line %d
caught Exception 14

Notice: Indirect modification of overloaded element of ArrayAccess@anonymous has no effect in %s on line %d
caught Exception 15

Notice: Indirect modification of overloaded element of ArrayAccess@anonymous has no effect in %s on line %d
caught Exception 16
caught Exception 17
caught Exception 18
caught Exception 19
caught Exception 20
caught Exception 21
caught Exception 22
caught Exception 23
caught Exception 24
caught Exception 25
caught Exception 26
caught Exception 27
caught Exception 28
caught Exception 29
caught Exception 30
caught Exception 31
caught Exception 32
