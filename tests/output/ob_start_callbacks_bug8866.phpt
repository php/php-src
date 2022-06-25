--TEST--
Test ob_start() with callbacks throwing an exception
--FILE--
<?php

ob_start(function ($buffer) {
    return '';
});
echo 'never printed - return \'\'';
ob_end_flush();

ob_start(function ($buffer) {
    throw new \Exception('x');
});
echo 'never printed - throw from callback';
try {
    ob_end_flush();
} catch (\Exception $e) {
    echo $e . "\n\n";
}

ob_start(function ($buffer) {
    new class() {
        public function __destruct()
        {
            throw new \Exception('y');
        }
    };

    throw new \Exception('y_ign'); // never reached
});
echo 'never printed - throw from destructor in callback (/wo assign)';
try {
    ob_end_flush();
} catch (\Exception $e) {
    echo $e . "\n\n";
}

ob_start(function ($buffer) {
    $o = new class() {
        public function __destruct()
        {
            throw new \Exception('z_simple');
        }
    };
});
echo 'never printed - throw from destructor in callback (/w assign simple)';
try {
    ob_end_flush();
} catch (\Exception $e) {
    echo $e . "\n\n";
}

ob_start(function ($buffer) {
    $o = new class() {
        public function __destruct()
        {
            throw new \Exception('z_destructor');
        }
    };

    throw new \Exception('z_double');
});
echo 'never printed - throw from destructor in callback (/w assign double)';
try {
    ob_end_flush();
} catch (\Exception $e) {
    echo $e . "\n\n";
}

var_dump(ob_get_level());

?>
--EXPECTF--
Exception: x in %s:10
Stack trace:
#0 [internal function]: {closure}('never printed -...', 9)
#1 %s(14): ob_end_flush()
#2 {main}

Exception: y in %s:23
Stack trace:
#0 %s(20): class@anonymous->__destruct()
#1 [internal function]: {closure}('never printed -...', 9)
#2 %s(31): ob_end_flush()
#3 {main}

Exception: z_simple in %s:40
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 %s(46): ob_end_flush()
#2 {main}

Exception: z_double in %s:59
Stack trace:
#0 [internal function]: {closure}('never printed -...', 9)
#1 %s(63): ob_end_flush()
#2 {main}

Next Exception: z_destructor in %s:55
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 %s(63): ob_end_flush()
#2 {main}

int(0)
