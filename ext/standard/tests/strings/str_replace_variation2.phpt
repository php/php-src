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
$subject = "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE \x5ACD abcd \xXYZ\tabcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)";

/* needles in an array to be compared in the string $string */
$search_str = [
    'Hello, World',
    '!!Hello, World',
    "??Hello, World",
    "$@#%^&*!~,.:;?",
    "123",
    "-1.2345",
    "abcd",
    'XYZ',
    "NULL",
    "0",
    "",
    " ",
    "\x000",
    "\x5AC",
    "\0000",
    ".3",
    "TRUE",
    "1",
    "FALSE",
    " ",
    "          ",
    'b',
    chr(128).chr(234).chr(65).chr(255).chr(256),
    $subject,
];

/* loop through to get the  $string */
for( $i = 0; $i < count($search_str); $i++ ) {
  echo "\n--- Iteration $i ---";
  echo "\n-- String after replacing the search value is => --\n";
  var_dump( str_replace($search_str[$i], "FOUND", $subject, $count) );
  echo "-- search string has found '$count' times\n";
}

?>
--EXPECT--
*** Testing str_replace() with various subjects ***
--- Iteration 0 ---
-- String after replacing the search value is => --
string(170) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!FOUND
        ?FOUND chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 1 ---
-- String after replacing the search value is => --
string(175) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: FOUND
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 2 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 3 ---
-- String after replacing the search value is => --
string(175) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $FOUND: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 4 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 5 ---
-- String after replacing the search value is => --
string(182) "Hello, world,0120333.3445FOUND67          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 6 ---
-- String after replacing the search value is => --
string(186) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD FOUND \xXYZ	FOUND $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 7 ---
-- String after replacing the search value is => --
string(186) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xFOUND	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 8 ---
-- String after replacing the search value is => --
string(185) "Hello, world,0120333.3445-1.234567          FOUND TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 9 ---
-- String after replacing the search value is => --
string(196) "Hello, world,FOUND12FOUND333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(FOUND).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 10 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 11 ---
-- String after replacing the search value is => --
string(300) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSEFOUNDZCDFOUNDabcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorld
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '29' times

--- Iteration 12 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 13 ---
-- String after replacing the search value is => --
string(187) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE FOUNDD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 14 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 15 ---
-- String after replacing the search value is => --
string(187) "Hello, world,0120333FOUND445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 16 ---
-- String after replacing the search value is => --
string(185) "Hello, world,0120333.3445-1.234567          NULL FOUND FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 17 ---
-- String after replacing the search value is => --
string(196) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 18 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FOUND ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 19 ---
-- String after replacing the search value is => --
string(300) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSEFOUNDZCDFOUNDabcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorld
FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '29' times

--- Iteration 20 ---
-- String after replacing the search value is => --
string(179) "Hello, world,0120333.3445-1.234567FOUNDNULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 21 ---
-- String after replacing the search value is => --
string(192) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD aFOUNDcd \xXYZ	aFOUNDcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 22 ---
-- String after replacing the search value is => --
string(184) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE ZCD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World
        ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 23 ---
-- String after replacing the search value is => --
string(5) "FOUND"
-- search string has found '1' times
