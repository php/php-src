--TEST--
GH-21639: Frameless calls keep volatile CV arguments alive during __toString()
--FILE--
<?php
class ImplodeElement {
    public function __toString(): string {
        global $separator, $pieces;

        $separator = null;
        $pieces = null;

        return "C";
    }
}

$separator = str_repeat(",", 1) . " ";
$pieces = [new ImplodeElement(), 42];

var_dump(implode($separator, $pieces));
var_dump($separator, $pieces);

class ImplodeElementWithoutSeparator {
    public function __toString(): string {
        global $oneArgPieces;

        $oneArgPieces = null;

        return "D";
    }
}

$oneArgPieces = [new ImplodeElementWithoutSeparator(), 42];

var_dump(implode($oneArgPieces));
var_dump($oneArgPieces);

class InArrayNeedle {
    public function __toString(): string {
        global $inArrayHaystack;

        $inArrayHaystack = null;

        return "needle";
    }
}

$inArrayHaystack = [new InArrayNeedle()];

var_dump(in_array("needle", $inArrayHaystack));
var_dump($inArrayHaystack);

class StrtrReplacement {
    public function __toString(): string {
        global $strtrReplacements;

        $strtrReplacements = null;

        return "b";
    }
}

$strtrReplacements = ["a" => new StrtrReplacement()];

var_dump(strtr("a", $strtrReplacements));
var_dump($strtrReplacements);

class StrReplaceSubject {
    public function __toString(): string {
        global $strReplaceSubject;

        $strReplaceSubject = null;

        return "a";
    }
}

$strReplaceSubject = [new StrReplaceSubject(), "aa"];

var_dump(str_replace("a", "b", $strReplaceSubject));
var_dump($strReplaceSubject);
?>
--EXPECT--
string(5) "C, 42"
NULL
NULL
string(3) "D42"
NULL
bool(true)
NULL
string(1) "b"
NULL
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(2) "bb"
}
NULL
