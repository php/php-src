--TEST--
resource typehint
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

function foo(resource $a) {
    var_dump($a); 
}
foo(1); // E_RECOVERABLE_ERROR
foo("1"); // E_RECOVERABLE_ERROR
foo(1.0); // E_RECOVERABLE_ERROR
foo(0); // E_RECOVERABLE_ERROR
foo("0"); // E_RECOVERABLE_ERROR
foo("1a"); // E_RECOVERABLE_ERROR
foo("a"); // E_RECOVERABLE_ERROR
foo(1.5); // E_RECOVERABLE_ERROR
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // E_RECOVERABLE_ERROR
foo(fopen("data:text/plain,foobar", "r")); // works
?>
--EXPECTF--
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, integer given, called in %s on line 15 and defined in %s on line 12
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 16 and defined in %s on line 12
string(1) "1"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, double given, called in %s on line 17 and defined in %s on line 12
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, integer given, called in %s on line 18 and defined in %s on line 12
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 19 and defined in %s on line 12
string(1) "0"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 20 and defined in %s on line 12
string(2) "1a"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 21 and defined in %s on line 12
string(1) "a"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, double given, called in %s on line 22 and defined in %s on line 12
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, array given, called in %s on line 23 and defined in %s on line 12
array(0) {
}
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, object given, called in %s on line 24 and defined in %s on line 12
object(stdClass)#%d (0) {
}
resource(%d) of type (stream)