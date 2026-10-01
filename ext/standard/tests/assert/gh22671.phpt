--TEST--
GH-22671 (assert.bail must not call zend_throw_unwind_exit with a pending exception)
--INI--
zend.assertions=1
assert.bail=1
assert.exception=0
assert.callback=cb
error_reporting=0
--FILE--
<?php
register_shutdown_function(function () {
    echo "shutdown reached\n";
});

class Inner extends Exception {
    public function __destruct() {
        throw new Exception("thrown from __destruct");
    }
}

class Boom extends Exception {
    public function __toString(): string {
        throw new Inner("inner");
    }
}

function cb() {
    throw new Boom("boom");
}

// The callback throws Boom; the bail path prints it, which re-throws from
// Boom::__toString(), and releasing that exception re-throws again from
// Inner::__destruct(). The bail path must not reach zend_throw_unwind_exit()
// while an exception is pending, otherwise the process aborts.
assert(false);

echo "not reached\n";
?>
--EXPECT--
shutdown reached
