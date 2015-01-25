--TEST--
Test nested function calls in strict_types=0 and strict_types=1 modes
--FILE--
<?php

$errored = FALSE;

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) use (&$errored) {
    // skipping errors make testing more practical
    if ($errno !== E_RECOVERABLE_ERROR) die("Wrong errno");
    
    echo "Catchable fatal error: $errstr in $errfile on line $errline\n";
    $errored = TRUE;
    return TRUE;
});

function takes_int(int $x) {
    global $errored;
    if ($errored) {
        echo "Failure!", PHP_EOL;
        $errored = FALSE;
    } else {
        echo "Success!", PHP_EOL;
    }
}

declare(strict_types=1) {
    function strict_calls_takes_int() {
        takes_int(1.0); // should fail, strict mode
    }

    class StrictTakesIntCaller {
        public function call() {
            takes_int(1.0); // should fail, strict mode
        }
    }
}

declare(strict_types=0) {
    function explicit_weak_calls_takes_int() {
        takes_int(1.0); // should succeed, weak mode
    }

    class ExplicitWeakTakesIntCaller {
        public function call() {
            takes_int(1.0); // should succeed, weak mode
        }
    }
}

// implicit weak mode code
function weak_calls_takes_int() {
    takes_int(1.0); // should succeed, weak mode
}

class WeakTakesIntCaller {
    public function call() {
        takes_int(1.0); // should succeed, weak mode
    }
}

// Now for the calling!

// implicit weak mode code
strict_calls_takes_int(); // should cause an error: our call to func is weak, but it was declared in strict mode so calls it makes are strict
weak_calls_takes_int(); // should succeed
explicit_weak_calls_takes_int(); // should succeed
(new StrictTakesIntCaller)->call(); // should cause an error
(new WeakTakesIntCaller)->call(); // should succeed
(new ExplicitWeakTakesIntCaller)->call(); // should succeed

declare(strict_types=0) {
    strict_calls_takes_int(); // should cause an error: our call to func is weak, but it was declared in strict mode so calls it makes are strict
    weak_calls_takes_int(); // should succeed
    explicit_weak_calls_takes_int(); // should succeed
    (new StrictTakesIntCaller)->call(); // should cause an error
    (new WeakTakesIntCaller)->call(); // should succeed
    (new ExplicitWeakTakesIntCaller)->call(); // should succeed
}

declare(strict_types=1) {
    strict_calls_takes_int(); // should cause an error 
    weak_calls_takes_int(); // should succeed: our call to func is strict, but it was declared in weak mode so calls it makes are weak
    explicit_weak_calls_takes_int(); // should succeed
    (new StrictTakesIntCaller)->call(); // should cause an error
    (new WeakTakesIntCaller)->call(); // should succeed
    (new ExplicitWeakTakesIntCaller)->call(); // should succeed
}

?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 26 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 31 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 26 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 31 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 26 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
Catchable fatal error: Argument 1 passed to takes_int() must be of the type integer, float given, called in %sstrict_nested.php on line 31 and defined in %sstrict_nested.php on line 14
Failure!
Success!
Success!
