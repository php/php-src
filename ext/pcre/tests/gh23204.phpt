--TEST--
GH-23204 (Use-after-free when __toString() destroys the array being read)
--FILE--
<?php
class UnsetPatterns implements Stringable {
    public function __toString(): string {
        global $patterns;
        $patterns = null;
        return "/a/";
    }
}

$patterns = [new UnsetPatterns, "/b/", "/c/", "/d/"];
var_dump(preg_replace($patterns, "z", "abcd"));
var_dump($patterns);

class UnsetReplacements implements Stringable {
    public function __toString(): string {
        global $replacements;
        $replacements = null;
        return "z";
    }
}

$replacements = [new UnsetReplacements, "y", "y", "y"];
var_dump(preg_replace(["/a/", "/b/", "/c/", "/d/"], $replacements, "abcd"));

class UnsetSubjects implements Stringable {
    public function __toString(): string {
        global $subjects;
        $subjects = null;
        return "abcd";
    }
}

$subjects = [new UnsetSubjects, "abcd"];
var_dump(preg_replace("/a/", "z", $subjects));

class AppendPattern implements Stringable {
    public function __toString(): string {
        global $appended;
        $appended[] = "/z/";
        return "/a/";
    }
}

$appended = [new AppendPattern, "/b/"];
var_dump(preg_replace($appended, "X", "abz"));
var_dump(count($appended));

class Boom implements Stringable {
    public function __toString(): string {
        global $boom;
        $boom = null;
        throw new Exception("boom");
    }
}

$boom = [new Boom, "/b/"];
try {
    preg_replace($boom, "X", "ab");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
string(4) "zzzz"
NULL
string(4) "zyyy"
array(2) {
  [0]=>
  string(4) "zbcd"
  [1]=>
  string(4) "zbcd"
}
string(3) "XXz"
int(3)
Exception: boom
