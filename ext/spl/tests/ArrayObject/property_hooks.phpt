--TEST--
ArrayObject with property hooks
--FILE--
<?php

class TestHooks
{
    private bool $isModified = false;
    public string $first {
        get {
            return strtoupper($this->first);
        }
    }

    public function __construct(string $first, public string $last) {
        $this->first = $first;
    }

    public string $fullName {
        // Override the "read" action with arbitrary logic.
        get => $this->first . " " . $this->last;

        // Override the "write" action with arbitrary logic.
        set {
            [$this->first, $this->last] = explode(' ', $value, 2);
            $this->isModified = true;
        }
    }

    public string $username {
        set(string $value) {
            if (strlen($value) > 10) throw new \Exception('Too long');
            $this->username = strtolower($value);
        }
    }
}

$o = new TestHooks('first', 'last');
$a = new ArrayObject($o);

echo 'Check object properties directly', PHP_EOL;
var_dump($o->first);
var_dump($o->last);
var_dump($o->fullName);

echo 'Check object properties via ArrayObject index', PHP_EOL;
var_dump($a['first']);
var_dump($a['last']);
var_dump($a['fullName']);
var_dump($a['username']);

echo 'Write to object properties via ArrayObject index', PHP_EOL;
$a['first'] = 'ArrayObject';
$a['last'] = 'ArrayObject';
$a['fullName'] = 'ArrayObject is hell';
$a['username'] = 'whatever_hooks_do_not_matter';

echo 'Check object properties directly', PHP_EOL;
var_dump($o->first);
var_dump($o->last);
var_dump($o->fullName);
var_dump($o->username);

?>
--EXPECTF--
Check object properties directly
string(5) "FIRST"
string(4) "last"
string(10) "FIRST last"
Check object properties via ArrayObject index
string(5) "first"
string(4) "last"

Warning: Undefined array key "fullName" in %s on line %d
NULL

Warning: Undefined array key "username" in %s on line %d
NULL
Write to object properties via ArrayObject index
Check object properties directly
string(11) "ARRAYOBJECT"
string(11) "ArrayObject"
string(23) "ARRAYOBJECT ArrayObject"
string(28) "whatever_hooks_do_not_matter"
