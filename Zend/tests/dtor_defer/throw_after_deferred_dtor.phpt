--TEST--
Deferred destructor: a throwing destructor pending at an explicit throw preempts it, whether dropped before the statement or during the throw operand's evaluation, so no spurious exception chain is created
--FILE--
<?php
class D {
    public function __destruct() { throw new Exception("dtor"); }
}

// The throw operand is a pre-existing object; the destructor is dropped before it.
function rethrow_case() {
    try {
        try {
            throw new Exception("original");
        } catch (Exception $orig) {
            $x = new D();
            unset($x);
            throw $orig;
        }
    } catch (Exception $e) {
        echo "rethrow: ", $e->getMessage(), " prev=", $e->getPrevious()?->getMessage() ?? "none", "\n";
    }
}
rethrow_case();

// The throw operand is a call that drops the destructor during its evaluation.
function make(Exception $e) {
    $x = new D();
    unset($x);
    return $e;
}
function operand_call_case() {
    $pre = new Exception("explicit");
    try {
        throw make($pre);
    } catch (Exception $e) {
        echo "operand-call: ", $e->getMessage(), " prev=", $e->getPrevious()?->getMessage() ?? "none", "\n";
    }
}
operand_call_case();
?>
--EXPECT--
rethrow: dtor prev=none
operand-call: dtor prev=none
