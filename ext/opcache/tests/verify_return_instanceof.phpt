--TEST--
Instanceof checks in VERIFY_RETURN_TYPE optimization may deal with unlinked classes
--FILE--
<?php
interface foo { }

interface biz {}

class qux implements foo {
    public function bar(): biz {
        $x = $this;
        return $x;
    }
}

?>
===DONE===
--EXPECT--
===DONE===
