--TEST--
Memory leak in preg_match() frameless function with invalid regex and object arguments
--FILE--
<?php
class Str {
    private $val;
    public function __construct($val) {
        $this->val = $val;
    }
    public function __toString() {
        return $this->val;
    }
}

$regex = new Str("invalid regex");
$subject = new Str("some subject");

// Running in a loop to ensure leak detection if run with memory tools
for ($i = 0; $i < 100; $i++) {
    @preg_match($regex, $subject);
}

echo "Done";
?>
--EXPECT--
Done
