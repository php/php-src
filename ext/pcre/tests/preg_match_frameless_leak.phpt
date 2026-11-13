--TEST--
Memory leak in preg_match() frameless function with invalid regex and object arguments
--EXTENSIONS--
pcre
zend_test
--FILE--
<?php
class Str {
    private $val;
    public function __construct($val) {
        $this->val = zend_return_non_interned_string($val);
    }
    public function __toString() {
        return $this->val;
    }
}

$regex = new Str("invalid regex");
$subject = new Str("some subject");

@preg_match($regex, $subject);

echo "Done";
?>
--EXPECT--
Done
