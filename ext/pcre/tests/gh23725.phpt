--TEST--
GH-23725 (Use-after-free when __toString() destroys a preg_replace() argument)
--FILE--
<?php
class UnsetPatterns implements Stringable {
    public function __toString(): string {
        global $patterns;
        $patterns = null;
        return "/a/";
    }
}

class UnsetReplacements implements Stringable {
    public function __toString(): string {
        global $replacements;
        $replacements = null;
        return "z";
    }
}

class UnsetSubjects implements Stringable {
    public function __toString(): string {
        global $subjects;
        $subjects = null;
        return "abc";
    }
}

class UnsetPatternString implements Stringable {
    public function __toString(): string {
        global $pattern;
        $pattern = null;
        return "z";
    }
}

class AppendPatterns implements Stringable {
    public function __toString(): string {
        global $patterns;
        $patterns[] = "/z/";
        return "/a/";
    }
}

class Boom implements Stringable {
    public function __toString(): string {
        global $patterns;
        $patterns = null;
        throw new Exception("boom");
    }
}

function destroyedPatternArray(): void {
    global $patterns;
    $patterns = [new UnsetPatterns, "/b/", "/c/"];
    echo "pattern array: ";
    var_dump(preg_replace($patterns, "z", "abc"));
    var_dump($patterns);
}

function destroyedReplacementArray(): void {
    global $replacements;
    $replacements = [new UnsetReplacements, "y", "y"];
    echo "replacement array: ";
    var_dump(preg_replace(["/a/", "/b/", "/c/"], $replacements, "abc"));
    var_dump($replacements);
}

function destroyedSubjectArray(): void {
    global $subjects;
    $subjects = [new UnsetSubjects, "abc"];
    echo "subject array: ";
    var_dump(preg_replace("/a/", "z", $subjects));
    var_dump($subjects);
}

function destroyedPatternString(): void {
    global $pattern;
    $sep = "/";
    $pattern = $sep . "a" . $sep;
    echo "pattern string: ";
    var_dump(preg_replace($pattern, new UnsetPatternString, "abc"));
    var_dump($pattern);
}

function appendedPatternArray(): void {
    global $patterns;
    $patterns = [new AppendPatterns, "/b/"];
    echo "appended: ";
    var_dump(preg_replace($patterns, "X", "abz"));
    echo "count: ", count($patterns), "\n";
}

function threw(): void {
    global $patterns;
    $patterns = [new Boom, "/b/"];
    try {
        var_dump(preg_replace($patterns, "X", "ab"));
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($patterns);
}

destroyedPatternArray();
destroyedReplacementArray();
destroyedSubjectArray();
destroyedPatternString();
appendedPatternArray();
threw();
?>
--EXPECT--
pattern array: string(3) "zzz"
NULL
replacement array: string(3) "zyy"
NULL
subject array: array(2) {
  [0]=>
  string(3) "zbc"
  [1]=>
  string(3) "zbc"
}
NULL
pattern string: string(3) "zbc"
NULL
appended: string(3) "XXz"
count: 3
Exception: boom
NULL
