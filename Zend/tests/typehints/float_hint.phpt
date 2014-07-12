--TEST--
float typehint
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

function foo(float $a) {
    var_dump($a); 
}
foo(1); // float(1)
foo("1"); // float(1)
foo(1.0); // float(1)
foo("1a"); // Notice: A non well formed numeric value encountered float(1)
foo("a"); // E_RECOVERABLE_ERROR
foo(1.5); // float(1.5)
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // Notice: Object of class stdClass could not be converted to float float(1)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
float(1)
float(1)
float(1)
E_NOTICE: A non well formed numeric value encountered in %s on line 12
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, string given, called in %s on line 19 and defined in %s on line 12
int(0)
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, array given, called in %s on line 21 and defined in %s on line 12
float(0)
E_NOTICE: Object of class stdClass could not be converted to double in %s on line 12
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, resource given, called in %s on line 23 and defined in %s on line 12
float(%d)
