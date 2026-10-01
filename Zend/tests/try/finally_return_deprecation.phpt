--TEST--
Returning from a finally block is deprecated
--FILE--
<?php

function withValue() {
    try {
    } finally {
        return 1;
    }
}

function withoutValue() {
    try {
    } finally {
        return;
    }
}

// Not deprecated: the return belongs to the try block, not the finally.
function inTry() {
    try {
        return 1;
    } finally {
    }
}

// Not deprecated: the return belongs to a catch block.
function inCatch() {
    try {
        throw new Exception();
    } catch (Exception $e) {
        return 1;
    } finally {
    }
}

// Not deprecated: the return belongs to a nested closure, not the finally.
function nestedClosure() {
    try {
    } finally {
        $f = function () {
            return 1;
        };
    }
}

// Deprecated twice: both returns are lexically inside the outer finally, even
// though one of them sits in a nested try block.
function nestedTry() {
    try {
    } finally {
        try {
            return 1;
        } finally {
            return 2;
        }
    }
}

// Deprecated once: only the closure's own finally return is flagged. The outer
// finally must not leak into the closure, so the closure's try return is left alone.
function closureWithTry() {
    try {
    } finally {
        $f = function () {
            try {
                return 1;
            } finally {
                return 2;
            }
        };
    }
}

?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d

Deprecated: Returning from a finally block is deprecated in %s on line %d

Deprecated: Returning from a finally block is deprecated in %s on line %d

Deprecated: Returning from a finally block is deprecated in %s on line %d

Deprecated: Returning from a finally block is deprecated in %s on line %d
