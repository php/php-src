--TEST--
GH-21639: Frameless preg_replace keeps volatile CV arguments alive during __toString()
--EXTENSIONS--
pcre
--FILE--
<?php
class PregReplaceSubject {
    public function __toString(): string {
        global $pregReplaceSubject;

        $pregReplaceSubject = null;

        return "a";
    }
}

$pregReplaceSubject = [new PregReplaceSubject(), "aa"];

var_dump(preg_replace("/a/", "b", $pregReplaceSubject));
var_dump($pregReplaceSubject);
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(2) "bb"
}
NULL
