--TEST--
Collator::getRules() resets errors and rejects unconstructed objects
--EXTENSIONS--
intl
--FILE--
<?php
class UnconstructedCollator extends Collator {
    public function __construct() {}
}

foreach ([fn($c) => $c->getRules(), collator_get_rules(...)] as $getRules) {
    $collator = new Collator('root');
    var_dump($collator->getAttribute(-1));
    var_dump(intl_get_error_code() === U_ILLEGAL_ARGUMENT_ERROR);
    var_dump($getRules($collator));
    var_dump(intl_get_error_message());
    var_dump($collator->getErrorMessage());

    try {
        $getRules(new UnconstructedCollator());
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
bool(false)
bool(true)
string(0) ""
string(12) "U_ZERO_ERROR"
string(12) "U_ZERO_ERROR"
Object not initialized
bool(false)
bool(true)
string(0) ""
string(12) "U_ZERO_ERROR"
string(12) "U_ZERO_ERROR"
Object not initialized
