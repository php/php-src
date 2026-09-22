--TEST--
Memory leak in preg_match() frameless function with invalid regex and object arguments
--FILE--
<?php
class Str {
    private $val;
    public function __construct($val) {
        $this->val = str_repeat($val, random_int(1, 1));
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
