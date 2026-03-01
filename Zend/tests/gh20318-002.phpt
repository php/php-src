--TEST--
GH-20318 002: Assign to ref: Concurrent reference source list mutations variations
--ENV--
LEN=10
--FILE--
<?php

function remove_single_ptr_source() {
    $obj = new class {
        public string $a = '';
        function __toString() {
            unset($this->a);
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $r = $obj;

    var_dump($obj, $r);
}

function replace_single_ptr_source_incompatible() {
    $obj = new class {
        public int|string $a = 0;
        public int $b = 0;
        public $c;
        function __toString() {
            unset($this->a);
            $this->b = &$this->c;
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $obj->c = &$r;
    try {
        $r = $obj;
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }

    var_dump($obj, $r);
}

function remove_current_source_in_list() {
    $obj = new class {
        public string $a = '';
        public string $b = '';
        function __toString() {
            unset($this->a);
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $obj->b = &$obj->a;
    $r = $obj;

    var_dump($obj, $r);
}

function remove_next_source_in_list() {
    $obj = new class {
        public string $a = '';
        public string $b = '';
        function __toString() {
            unset($this->b);
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $obj->b = &$obj->a;
    $r = $obj;

    var_dump($obj, $r);
}

function remove_all_sources_in_list() {
    $obj = new class {
        public string $a = '';
        public string $b = '';
        function __toString() {
            unset($this->a);
            unset($this->b);
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $obj->b = &$obj->a;
    $r = $obj;

    var_dump($obj, $r);
}

function add_sources() {
    $obj = new class {
        public string $a = '';
        public string $b = '';
        public string $c = '';
        public string $d = '';
        public string $e = '';
        public string $f = '';
        public string $g = '';
        public string $h = '';
        function __toString() {
            var_dump(__METHOD__);
            $this->b = &$this->a;
            $this->c = &$this->a;
            $this->d = &$this->a;
            $this->e = &$this->a;
            $this->f = &$this->a;
            $this->g = &$this->a;
            $this->h = &$this->a;
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    $r = $obj;

    var_dump($obj, $r);
}

function cleanup_shrink() {
    $obj = new class {
        public string $a = '';
    };

    $r = &$obj->a;

    $objs = [];
    for ($i = 0; $i < 100; $i++) {
        $objs[] = clone $obj;
    }

    $r = new class($objs) {
        function __construct(public mixed &$objs) {}
        function __toString() {
            $this->objs = array_slice($this->objs, 0, 2);
            return str_repeat('a', getenv('LEN'));
        }
    };

    var_dump($obj, $r);
}

function add_incompatible() {
    $obj = new class {
        public int|string $a = 1;
        public int $b = 2;
        function __toString() {
            $this->b = &$this->a;
            return str_repeat('a', getenv('LEN'));
        }
    };

    $r = &$obj->a;
    try {
        $r = $obj;
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }

    var_dump($obj, $r);
}

foreach ([
    'remove_single_ptr_source',
    'replace_single_ptr_source_incompatible',
    'remove_current_source_in_list',
    'remove_next_source_in_list',
    'remove_all_sources_in_list',
    'cleanup_shrink',
    'add_incompatible',
] as $func) {
    echo "# ", $func, ":\n";
    $func();
}

?>
==DONE==
--EXPECT--
# remove_single_ptr_source:
object(class@anonymous)#1 (0) {
  ["a"]=>
  uninitialized(string)
}
string(10) "aaaaaaaaaa"
# replace_single_ptr_source_incompatible:
TypeError: Cannot assign class@anonymous to reference held by property class@anonymous::$b of type int
object(class@anonymous)#1 (2) {
  ["a"]=>
  uninitialized(string|int)
  ["b"]=>
  &int(0)
  ["c"]=>
  &int(0)
}
int(0)
# remove_current_source_in_list:
object(class@anonymous)#2 (1) {
  ["a"]=>
  uninitialized(string)
  ["b"]=>
  &string(10) "aaaaaaaaaa"
}
string(10) "aaaaaaaaaa"
# remove_next_source_in_list:
object(class@anonymous)#2 (1) {
  ["a"]=>
  &string(10) "aaaaaaaaaa"
  ["b"]=>
  uninitialized(string)
}
string(10) "aaaaaaaaaa"
# remove_all_sources_in_list:
object(class@anonymous)#2 (0) {
  ["a"]=>
  uninitialized(string)
  ["b"]=>
  uninitialized(string)
}
string(10) "aaaaaaaaaa"
# cleanup_shrink:
object(class@anonymous)#2 (1) {
  ["a"]=>
  &string(10) "aaaaaaaaaa"
}
string(10) "aaaaaaaaaa"
# add_incompatible:
TypeError: Cannot assign class@anonymous to reference held by property class@anonymous::$b of type int
object(class@anonymous)#3 (2) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
int(1)
==DONE==
