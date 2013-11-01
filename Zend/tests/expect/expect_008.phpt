--TEST--
test disabled expectations have no ill side effects
--INI--
zend.expectations=0
--FILE--
<?php
class One {
    public function __construct() {
        expect !$this || 0;
    }
}
class Two extends One {}

class OdEar extends ExpectationException {}

function blah(){ return 1; }

$variable = 1;
expect true : "constant message";
expect ($variable && $variable) || 
            php_sapi_name() : new OdEar("constant message");
expect false;
expect blah() : blah();

new Two();
new Two();
new Two();

expect blah() || blah() || blah() : blah() || blah() || blah() || blah();

var_dump(true);
?>
--EXPECT--	
bool(true)
