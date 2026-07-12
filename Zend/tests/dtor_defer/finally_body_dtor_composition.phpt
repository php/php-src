--TEST--
Deferred destructor: a destructor dropped in a finally body composes with a destructor exception pending from the protected block and is caught by the enclosing try
--FILE--
<?php
class D {
    public function __construct(public string $id) {}
    public function __destruct() { throw new Exception("dtor {$this->id}"); }
}
function f() {
    try {
        try {
            $a = new D("try");
            unset($a);
            return "unreached";
        } finally {
            $b = new D("finally");
            unset($b);
            echo "finally body ran\n";
        }
    } catch (Exception $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
        echo "previous: ", $e->getPrevious()?->getMessage() ?? "none", "\n";
        return "caught";
    }
}
var_dump(f());
?>
--EXPECT--
finally body ran
Exception: dtor finally
previous: dtor try
string(6) "caught"
