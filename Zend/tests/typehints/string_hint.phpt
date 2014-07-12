--TEST--
string typehint
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

function foo(string $a) {
    var_dump($a); 
}
foo(1); // string "1"
foo("1"); // string "1"
foo(1.0); // string "1"
foo("1a"); // string "1a"
foo("a"); // string "a"
foo(1.5); // string "1.5"
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // E_RECOVERABLE_ERROR
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
string(1) "1"
string(1) "1"
string(1) "1"
string(2) "1a"
string(1) "a"
string(3) "1.5"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, array given, called in %s on line 21 and defined in %s on line 12
string(5) "Array"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, object given, called in %s on line 22 and defined in %s on line 12
string(6) "Object"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, resource given, called in %s on line 23 and defined in %s on line 12
string(%d) "%s"
