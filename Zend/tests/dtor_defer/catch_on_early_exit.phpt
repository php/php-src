--TEST--
Deferred destructor: a throwing destructor dropped in a try is caught by that try when the try is left via return/break/continue
--FILE--
<?php
class D {
    public function __construct(public string $id) {}
    public function __destruct() { throw new Exception("dtor {$this->id}"); }
}

function via_return() {
    try {
        $d = new D("r");
        unset($d);
        return "returned";
    } catch (Exception $e) {
        echo "return-case caught: {$e->getMessage()}\n";
        return "caught";
    }
}
var_dump(via_return());

function via_break() {
    foreach ([1] as $v) {
        try {
            $d = new D("b");
            unset($d);
            break;
        } catch (Exception $e) {
            echo "break-case caught: {$e->getMessage()}\n";
        }
    }
    return "done";
}
var_dump(via_break());

function via_continue() {
    foreach ([1, 2] as $v) {
        try {
            $d = new D("c$v");
            unset($d);
            continue;
        } catch (Exception $e) {
            echo "continue-case caught: {$e->getMessage()}\n";
        }
    }
    return "done";
}
var_dump(via_continue());
?>
--EXPECT--
return-case caught: dtor r
string(6) "caught"
break-case caught: dtor b
string(4) "done"
continue-case caught: dtor c1
continue-case caught: dtor c2
string(4) "done"
