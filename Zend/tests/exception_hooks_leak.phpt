--TEST--
Overridden Exception get hooks don't leak
--CREDITS--
Saad Fellahi (TarPeg007)
--FILE--
<?php

class E extends Exception {
    public $code = 0 { get => $this->code . '!'; }
    public string $file { get => $this->file . '!'; }
    public $message { get => $this->message . '!'; }
}

$e = new E('Foo');
var_dump($e->getCode());
var_dump($e->getFile());
var_dump($e->getMessage());
var_dump($e->getTrace());
echo $e, "\n";

class E2 extends Exception {
    public $code = 0 {
        &get {
            $value = $this->code . '!';
            return $value;
        }
    }
}
$e2 = new E2('Foo');
var_dump($e2->getCode());

?>
--EXPECTF--
string(2) "0!"
string(%d) "%sexception_hooks_leak.php!"
string(4) "Foo!"
array(0) {
}
E: Foo! in %s:%d
Stack trace:
#0 {main}
string(2) "0!"
