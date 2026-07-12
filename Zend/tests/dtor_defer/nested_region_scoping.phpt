--TEST--
Deferred destructor: nested try regions scope the deferred queue; each region's destructor surfaces at its own level and a finally-return discards only its own
--FILE--
<?php
class D {
    public function __construct(public string $id) {}
    public function __destruct() { throw new Exception("dtor {$this->id}"); }
}

function inner_level() {
    try {
        try {
            $d = new D("inner");
            unset($d);
            return "inner-try-return";
        } catch (Exception $e) {
            echo "inner caught: {$e->getMessage()}\n";
            return "inner-catch";
        }
    } catch (Exception $e) {
        echo "outer WRONGLY caught: {$e->getMessage()}\n";
        return "outer-catch";
    }
}
var_dump(inner_level());

function outer_survives() {
    try {
        $d = new D("outer");
        unset($d);
        try {
            return "inner-try-return";
        } finally {
            return "inner-finally-return";
        }
    } catch (Exception $e) {
        echo "outer caught: {$e->getMessage()}\n";
        return "outer-catch";
    }
}
var_dump(outer_survives());

function finally_discards() {
    try {
        $d = new D("discarded");
        unset($d);
        return "try-return";
    } finally {
        return "finally-return";
    }
}
var_dump(finally_discards());
?>
--EXPECT--
inner caught: dtor inner
string(11) "inner-catch"
outer caught: dtor outer
string(11) "outer-catch"
string(14) "finally-return"
