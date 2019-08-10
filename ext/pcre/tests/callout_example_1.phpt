--TEST--
Auto callout tracing
--FILE--
<?php

$pattern = '/^(?:a(?=a*(\1?+b)))+\1$/C';
$subject = 'aaabbb';

preg_callout_function(function($info) use($pattern) {
    $subject = $info['subject'];
    echo $pattern . " against " . $subject, "\n";
    $patternPos = $info['pattern_position'];
    $subjectPos = $info['current_position'];
    echo str_pad(str_repeat(' ', $patternPos) . '^', strlen($pattern));
    echo str_repeat(' ', strlen(' against '));
    echo str_pad(str_repeat(' ', $subjectPos) . '^', strlen($subject) + 1);

    $captures = [];
    foreach ($info['captures'] as $i => $capture) {
        if ($capture[0] === null) {
            $captures[] = "_";
            continue;
        }

        $last = $i === $info['capture_last'] ? '(*) ' : '';
        $captures[] = $last . '"' . $capture[0] . '" @ ' . $capture[1];
    }
    echo "  [", implode(", ", $captures), "]\n";

    return 0;
});
var_dump(preg_match($pattern, $subject));

?>
--EXPECT--
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
^                                  ^        [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
 ^                                 ^        [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
    ^                              ^        [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
     ^                              ^       [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
        ^                           ^       [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
          ^                           ^     [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
           ^                          ^     [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
               ^                      ^     [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                ^                      ^    [_]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                 ^                     ^    [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                  ^                 ^       [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
    ^                               ^       [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
     ^                               ^      [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
        ^                            ^      [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
          ^                           ^     [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
           ^                          ^     [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
               ^                       ^    [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                ^                       ^   [_, (*) "b" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                 ^                      ^   [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                  ^                  ^      [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
    ^                                ^      [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
     ^                                ^     [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
        ^                             ^     [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
          ^                           ^     [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
           ^                          ^     [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
               ^                        ^   [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                ^                        ^  [_, (*) "bb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                 ^                       ^  [_, (*) "bbb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                  ^                   ^     [_, (*) "bbb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
    ^                                 ^     [_, (*) "bbb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                    ^                 ^     [_, (*) "bbb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                      ^                  ^  [_, (*) "bbb" @ 3]
/^(?:a(?=a*(\1?+b)))+\1$/C against aaabbb
                       ^                 ^  [_, (*) "bbb" @ 3]
int(1)
