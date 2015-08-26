--TEST--
Array access on non-array
--FILE--
<?php
$values = array(
    'null' => null,
    'int' => 42,
    'float' => 3.14159,
    'bool' => true,
    'string' => 'hello world',
    'object' => new StdClass(),
);

foreach ($values as $type => $a) {
    print "\n\n--- {$type} ---\n";
    try {
        var_dump($a[0]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($a[0][1]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($a['foo']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($a['foo']['bar']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset($a[0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset($a[0][0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }
    unset($a, $b);
}

foreach ($values as $type => $a) {
    print "\n\n--- obj->{$type} ---\n";
    $obj = new StdClass();
    $obj->test = $a;

    try {
        var_dump($obj->test[0]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($obj->test[0][1]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($obj->test['foo']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump($obj->test['foo']['bar']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset($obj->test[0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset($obj->test[0][0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }
    unset($a, $b);
}

function returnInput($input) {
    return $input;
}

foreach ($values as $type => $a) {
    print "\n\n--- func() {$type} ---\n";
    try {
        var_dump(returnInput($a)[0]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump(returnInput($a)[0][1]);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump(returnInput($a)['foo']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        var_dump(returnInput($a)['foo']['bar']);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset(returnInput($a)[0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }

    try {
        $b = isset(returnInput($a)[0][0]);
        var_dump($b);
    } catch (Error $e) {
        print "Error: ". $e->getMessage() ." on line ". $e->getLine() ."\n";
    }
    unset($a, $b);
}



--EXPECTF--
--- null ---

Notice: Trying to get index of a non-array in %s on line 14
NULL

Notice: Trying to get index of a non-array in %s on line 20

Notice: Trying to get index of a non-array in %s on line 20
NULL

Notice: Trying to get index of a non-array in %s on line 26
NULL

Notice: Trying to get index of a non-array in %s on line 32

Notice: Trying to get index of a non-array in %s on line 32
NULL
bool(false)
bool(false)


--- int ---

Notice: Trying to get index of a non-array in %s on line 14
NULL

Notice: Trying to get index of a non-array in %s on line 20

Notice: Trying to get index of a non-array in %s on line 20
NULL

Notice: Trying to get index of a non-array in %s on line 26
NULL

Notice: Trying to get index of a non-array in %s on line 32

Notice: Trying to get index of a non-array in %s on line 32
NULL
bool(false)
bool(false)


--- float ---

Notice: Trying to get index of a non-array in %s on line 14
NULL

Notice: Trying to get index of a non-array in %s on line 20

Notice: Trying to get index of a non-array in %s on line 20
NULL

Notice: Trying to get index of a non-array in %s on line 26
NULL

Notice: Trying to get index of a non-array in %s on line 32

Notice: Trying to get index of a non-array in %s on line 32
NULL
bool(false)
bool(false)


--- bool ---

Notice: Trying to get index of a non-array in %s on line 14
NULL

Notice: Trying to get index of a non-array in %s on line 20

Notice: Trying to get index of a non-array in %s on line 20
NULL

Notice: Trying to get index of a non-array in %s on line 26
NULL

Notice: Trying to get index of a non-array in %s on line 32

Notice: Trying to get index of a non-array in %s on line 32
NULL
bool(false)
bool(false)


--- string ---
string(1) "h"

Notice: Uninitialized string offset: 1 in %s on line 20
string(0) ""

Warning: Illegal string offset 'foo' in %s on line 26
string(1) "h"

Warning: Illegal string offset 'foo' in %s on line 32

Warning: Illegal string offset 'bar' in %s on line 32
string(1) "h"
bool(true)
bool(true)


--- object ---
Error: Cannot use object of type stdClass as array on line 14
Error: Cannot use object of type stdClass as array on line 20
Error: Cannot use object of type stdClass as array on line 26
Error: Cannot use object of type stdClass as array on line 32
Error: Cannot use object of type stdClass as array on line 38
Error: Cannot use object of type stdClass as array on line 45


--- obj->null ---

Notice: Trying to get index of a non-array in %s on line 59
NULL

Notice: Trying to get index of a non-array in %s on line 65

Notice: Trying to get index of a non-array in %s on line 65
NULL

Notice: Trying to get index of a non-array in %s on line 71
NULL

Notice: Trying to get index of a non-array in %s on line 77

Notice: Trying to get index of a non-array in %s on line 77
NULL
bool(false)
bool(false)


--- obj->int ---

Notice: Trying to get index of a non-array in %s on line 59
NULL

Notice: Trying to get index of a non-array in %s on line 65

Notice: Trying to get index of a non-array in %s on line 65
NULL

Notice: Trying to get index of a non-array in %s on line 71
NULL

Notice: Trying to get index of a non-array in %s on line 77

Notice: Trying to get index of a non-array in %s on line 77
NULL
bool(false)
bool(false)


--- obj->float ---

Notice: Trying to get index of a non-array in %s on line 59
NULL

Notice: Trying to get index of a non-array in %s on line 65

Notice: Trying to get index of a non-array in %s on line 65
NULL

Notice: Trying to get index of a non-array in %s on line 71
NULL

Notice: Trying to get index of a non-array in %s on line 77

Notice: Trying to get index of a non-array in %s on line 77
NULL
bool(false)
bool(false)


--- obj->bool ---

Notice: Trying to get index of a non-array in %s on line 59
NULL

Notice: Trying to get index of a non-array in %s on line 65

Notice: Trying to get index of a non-array in %s on line 65
NULL

Notice: Trying to get index of a non-array in %s on line 71
NULL

Notice: Trying to get index of a non-array in %s on line 77

Notice: Trying to get index of a non-array in %s on line 77
NULL
bool(false)
bool(false)


--- obj->string ---
string(1) "h"

Notice: Uninitialized string offset: 1 in %s on line 65
string(0) ""

Warning: Illegal string offset 'foo' in %s on line 71
string(1) "h"

Warning: Illegal string offset 'foo' in %s on line 77

Warning: Illegal string offset 'bar' in %s on line 77
string(1) "h"
bool(true)
bool(true)


--- obj->object ---
Error: Cannot use object of type stdClass as array on line 59
Error: Cannot use object of type stdClass as array on line 65
Error: Cannot use object of type stdClass as array on line 71
Error: Cannot use object of type stdClass as array on line 77
Error: Cannot use object of type stdClass as array on line 83
Error: Cannot use object of type stdClass as array on line 90


--- func() null ---

Notice: Trying to get index of a non-array in %s on line 105
NULL

Notice: Trying to get index of a non-array in %s on line 111

Notice: Trying to get index of a non-array in %s on line 111
NULL

Notice: Trying to get index of a non-array in %s on line 117
NULL

Notice: Trying to get index of a non-array in %s on line 123

Notice: Trying to get index of a non-array in %s on line 123
NULL
bool(false)
bool(false)


--- func() int ---

Notice: Trying to get index of a non-array in %s on line 105
NULL

Notice: Trying to get index of a non-array in %s on line 111

Notice: Trying to get index of a non-array in %s on line 111
NULL

Notice: Trying to get index of a non-array in %s on line 117
NULL

Notice: Trying to get index of a non-array in %s on line 123

Notice: Trying to get index of a non-array in %s on line 123
NULL
bool(false)
bool(false)


--- func() float ---

Notice: Trying to get index of a non-array in %s on line 105
NULL

Notice: Trying to get index of a non-array in %s on line 111

Notice: Trying to get index of a non-array in %s on line 111
NULL

Notice: Trying to get index of a non-array in %s on line 117
NULL

Notice: Trying to get index of a non-array in %s on line 123

Notice: Trying to get index of a non-array in %s on line 123
NULL
bool(false)
bool(false)


--- func() bool ---

Notice: Trying to get index of a non-array in %s on line 105
NULL

Notice: Trying to get index of a non-array in %s on line 111

Notice: Trying to get index of a non-array in %s on line 111
NULL

Notice: Trying to get index of a non-array in %s on line 117
NULL

Notice: Trying to get index of a non-array in %s on line 123

Notice: Trying to get index of a non-array in %s on line 123
NULL
bool(false)
bool(false)


--- func() string ---
string(1) "h"

Notice: Uninitialized string offset: 1 in %s on line 111
string(0) ""

Warning: Illegal string offset 'foo' in %s on line 117
string(1) "h"

Warning: Illegal string offset 'foo' in %s on line 123

Warning: Illegal string offset 'bar' in %s on line 123
string(1) "h"
bool(true)
bool(true)


--- func() object ---
Error: Cannot use object of type stdClass as array on line 105
Error: Cannot use object of type stdClass as array on line 111
Error: Cannot use object of type stdClass as array on line 117
Error: Cannot use object of type stdClass as array on line 123
Error: Cannot use object of type stdClass as array on line 129
Error: Cannot use object of type stdClass as array on line 136
