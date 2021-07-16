--TEST--
Test str_replace() function
--INI--
precision=14
--FILE--
<?php
/*
  Description: Replace all occurrences of the search string with
               the replacement string
*/

echo "\n*** Testing str_replace() with various subjects ***";
$subject = "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE\000
        \x000\x5ACD\0abcd \xXYZ\tabcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)";

/* needles in an array to be compared in the string $string */
$search_str = array (
  "Hello, World",
  'Hello, World',
  '!!Hello, World',
  "??Hello, World",
  "$@#%^&*!~,.:;?",
  "123",
  123,
  "-1.2345",
  -1.2344,
  "abcd",
  'XYZ',
  "NULL",
  "0",
  0,
  "",
  " ",
  "\0",
  "\x000",
  "\x5AC",
  "\0000",
  ".3",
  TRUE,
  "TRUE",
  "1",
  1,
  FALSE,
  "FALSE",
  " ",
  "          ",
  'b',
  '\t',
  "\t",
  chr(128).chr(234).chr(65).chr(255).chr(256),
  $subject
);

/* loop through to get the  $string */
for( $i = 0; $i < count($search_str); $i++ ) {
  echo "\n--- Iteration $i ---";
  echo "\n-- String after replacing the search value is => --\n";
  var_dump( str_replace($search_str[$i], "FOUND", $subject, $count) );
  echo "-- search string has found '$count' times\n";
}

?>
--EXPECTF--
*** Testing str_replace() with various subjects ***
--- Iteration 0 ---
-- String after replacing the search value is => --
string(181) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!FOUND
        ?FOUND chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 1 ---
-- String after replacing the search value is => --
string(181) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!FOUND
        ?FOUND chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 2 ---
-- String after replacing the search value is => --
string(186) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: FOUND
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 3 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 4 ---
-- String after replacing the search value is => --
string(186) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $FOUND: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 5 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 6 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 7 ---
-- String after replacing the search value is => --
string(193) "Hello, world,0120333.3445FOUND67          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 8 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 9 ---
-- String after replacing the search value is => --
string(197) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0FOUND \xXYZ	FOUND $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 10 ---
-- String after replacing the search value is => --
string(197) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xFOUND	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 11 ---
-- String after replacing the search value is => --
string(196) "Hello, world,0120333.3445-1.234567          FOUND TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 12 ---
-- String after replacing the search value is => --
string(211) "Hello, world,FOUND12FOUND333.3445-1.234567          NULL TRUE FALSE%0
        %0FOUNDZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(FOUND).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '4' times

--- Iteration 13 ---
-- String after replacing the search value is => --
string(211) "Hello, world,FOUND12FOUND333.3445-1.234567          NULL TRUE FALSE%0
        %0FOUNDZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(FOUND).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '4' times

--- Iteration 14 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 15 ---
-- String after replacing the search value is => --
string(335) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSE%0
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND%00ZCD%0abcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorld
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '35' times

--- Iteration 16 ---
-- String after replacing the search value is => --
string(207) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSEFOUND
        FOUND0ZCDFOUNDabcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 17 ---
-- String after replacing the search value is => --
string(198) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        FOUNDZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 18 ---
-- String after replacing the search value is => --
string(198) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00FOUNDD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 19 ---
-- String after replacing the search value is => --
string(198) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        FOUNDZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 20 ---
-- String after replacing the search value is => --
string(198) "Hello, world,0120333FOUND445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 21 ---
-- String after replacing the search value is => --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(207) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 22 ---
-- String after replacing the search value is => --
string(196) "Hello, world,0120333.3445-1.234567          NULL FOUND FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 23 ---
-- String after replacing the search value is => --
string(207) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 24 ---
-- String after replacing the search value is => --
string(207) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 25 ---
-- String after replacing the search value is => --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 26 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FOUND%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 27 ---
-- String after replacing the search value is => --
string(335) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSE%0
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND%00ZCD%0abcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorld
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '35' times

--- Iteration 28 ---
-- String after replacing the search value is => --
string(190) "Hello, world,0120333.3445-1.234567FOUNDNULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 29 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0aFOUNDcd \xXYZ	aFOUNDcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 30 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 31 ---
-- String after replacing the search value is => --
string(199) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZFOUNDabcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 32 ---
-- String after replacing the search value is => --
string(195) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE%0
        %00ZCD%0abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 33 ---
-- String after replacing the search value is => --
string(5) "FOUND"
-- search string has found '1' times
