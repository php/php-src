--TEST--
test disabled expectations have no ill side effects
--INI--
zend.assertions=0
assert.exception=1
--FILE--
<?php
class One {
    public function __construct() {
        assert($this || 0);
    }
}
class Two extends One {}

class OdEar extends AssertionException {}

function blah(){ return 1; }

$variable = 1;
assert(true, "constant message");
assert(($variable && $variable) || php_sapi_name(), new OdEar("constant message"));
assert(false);
assert(blah(), blah());

new Two();
new Two();
new Two();

assert (blah() || blah() || blah(), blah() || blah() || blah() || blah());

var_dump(true);
?>
--EXPECT--
bool(true)
