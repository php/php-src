--TEST--
ob_start(): Check behaviour with deprecation converted to exception
--FILE--
<?php

class NotStringable {
    public function __construct(public string $val) {}
}
class IsStringable {
    public function __construct(public string $val) {}
    public function __toString() {
        return __CLASS__ . ": " . $this->val;
    }
}

$log = [];

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

function return_null($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return null;
}

function return_false($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return false;
}

function return_true($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return true;
}

function return_zero($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return 0;
}

function return_non_stringable($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return new NotStringable($string);
}

function return_stringable($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return new IsStringable($string);
}

ob_start('return_null');
ob_start('return_false');
ob_start('return_true');
ob_start('return_zero');
ob_start('return_non_stringable');
ob_start('return_stringable');

echo "In all of them\n\n";
try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_stringable handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_non_stringable handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_zero handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_true handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_false handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended return_null handler\n\n";

echo "All handlers are over\n\n";
echo implode("\n", $log);

?>
--EXPECT--
ob_end_flush(): Returning a non-string result from user output handler return_null is deprecated
Ended return_null handler

All handlers are over

return_stringable: <<<In all of them

>>>
return_non_stringable: <<<ob_end_flush(): Returning a non-string result from user output handler return_stringable is deprecated
Ended return_stringable handler

>>>
return_zero: <<<ob_end_flush(): Returning a non-string result from user output handler return_non_stringable is deprecated
Ended return_non_stringable handler

>>>
return_true: <<<0ob_end_flush(): Returning a non-string result from user output handler return_zero is deprecated
Ended return_zero handler

>>>
return_false: <<<ob_end_flush(): Returning a non-string result from user output handler return_true is deprecated
Ended return_true handler

>>>
return_null: <<<ob_end_flush(): Returning a non-string result from user output handler return_true is deprecated
Ended return_true handler

ob_end_flush(): Returning a non-string result from user output handler return_false is deprecated
Ended return_false handler

>>>
