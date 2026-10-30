--TEST--
Enum unserialize runs the autoloader outside of the current unserialize() context
--FILE--
<?php

class Init {
    public bool $ready = false;

    public function __unserialize(array $data): void {
        $this->ready = true;
    }
}

enum Backed: string {
    case Foo = Value::FOO;
}

spl_autoload_register(function ($class) {
    var_dump(unserialize('O:4:"Init":0:{}')->ready);
    eval(match ($class) {
        'Pure' => 'enum Pure { case Foo; }',
        'Value' => 'class Value { const FOO = "foo"; }',
    });
});

foreach (['E:8:"Pure:Foo";', 'E:10:"Backed:Foo";'] as $enum) {
    $data = unserialize('a:3:{i:0;' . $enum . 'i:1;O:8:"stdClass":0:{}i:2;r:3;}');
    var_dump($data[1] === $data[2]);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
