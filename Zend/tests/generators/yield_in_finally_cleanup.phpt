--TEST--
Free pending exceptions / return values on clone on yield in finally
--FILE--
<?php
function gen1() {
    try {
        throw new Exception();
    } finally {
        yield;
    }
}
function gen2() {
    try {
        $bar = "bar";
        return "foo" . $bar;
    } finally {
        yield;
    }
}
function gen3() {
    try {
        throw new Exception();
    } finally {
        try {
            $bar = "bar";
            return "foo" . $bar;
        } finally {
            yield;
        }
    }
}
function gen4() {
    try {
        try {
            $bar = "bar";
            return "foo" . $bar;
        } finally {
            yield;
        }
    } finally {
    }
}
gen1()->rewind();
gen2()->rewind();
gen3()->rewind();
gen4()->rewind();

?>
===DONE===
--EXPECT--
===DONE===
