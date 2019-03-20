--TEST--
PCRE partial matching
--FILE--
<?php

function test($regex, $subject, $offsetCapture = false) {
    $withOffsetCapture = $offsetCapture ? "with offset capture" : "";
    echo "Match $regex against $subject$withOffsetCapture:\n";
    $retval = preg_match($regex, $subject, $matches, $offsetCapture ? PREG_OFFSET_CAPTURE : 0);
    if ($retval === 0) {
        echo "No match.\n";
    } else if ($retval === 1) {
        if (preg_last_error() === PREG_PARTIAL_MATCH_ERROR) {
            echo "Partial match: ";
        } else {
            echo "Full match: ";
        }
        var_dump($matches);
    } else {
        echo "Error: " . preg_last_error() . "\n";
    }
    echo "\n";
}

test('/dog(sbody)?/p', 'dog');
test('/dog(sbody)?/P', 'dog');

test('/baz(?<=barbaz)(?=quux)/p', 'abcfoobarbazqu');
test('/abc(?<=äöüabc)(?=quux)/p', 'foobaräöüabcqu');

test('/abc(?<=äöüabc)(?=quux)/p', 'foobaräöüabcqu', true);

?>
--EXPECT--
Match /dog(sbody)?/p against dog:
Full match: array(1) {
  [0]=>
  string(3) "dog"
}

Match /dog(sbody)?/P against dog:
Partial match: array(2) {
  [0]=>
  string(3) "dog"
  [1]=>
  string(3) "dog"
}

Match /baz(?<=barbaz)(?=quux)/p against abcfoobarbazqu:
Partial match: array(2) {
  [0]=>
  string(5) "bazqu"
  [1]=>
  string(11) "foobarbazqu"
}

Match /abc(?<=äöüabc)(?=quux)/p against foobaräöüabcqu:
Partial match: array(2) {
  [0]=>
  string(5) "abcqu"
  [1]=>
  string(14) "baräöüabcqu"
}

Match /abc(?<=äöüabc)(?=quux)/p against foobaräöüabcquwith offset capture:
Partial match: array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "abcqu"
    [1]=>
    int(12)
  }
  [1]=>
  array(2) {
    [0]=>
    string(14) "baräöüabcqu"
    [1]=>
    int(3)
  }
}
