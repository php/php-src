--TEST--
bool typehint
--FILE--
<?php
$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_RECOVERABLE_ERROR => 'E_RECOVERABLE_ERROR'
];
set_error_handler(function (int $errno, string $errmsg, string $file, int $line) use ($errnames) {
    echo "$errnames[$errno]: $errmsg in $file on line $line\n";
    return true;
});

function foo(bool $a) {
    var_dump($a); 
}
foo(1); // bool(true)
foo("1"); // bool(true)
foo(1.0); // bool(true)
foo(0); // bool(false)
foo("0"); // bool(false)
foo("1a"); // bool(true)
foo("a"); // bool(true)
foo(1.5); // bool(true)
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // bool(true)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type boolean, array given, called in %s on line 23 and defined in %s on line 12
bool(false)
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type boolean, resource given, called in %s on line 25 and defined in %s on line 12
bool(true)