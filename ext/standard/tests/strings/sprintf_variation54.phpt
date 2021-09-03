--TEST--
sprintf() formats with different types
--FILE--
<?php

$formats = ['s', 'd', 'u', 'f', 'c', 'x'];
$values = [null, false, true, 2, 3.5, "foo", [], [1], fopen(__FILE__, "r"), new stdClass];

foreach ($formats as $format) {
    foreach ($values as $value) {
        echo "$format with " . (is_resource($value) ? "resource" : json_encode($value)) . ":\n";
        try {
            echo sprintf("%" . $format, $value), "\n";
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        echo "\n";
    }
}

?>
--EXPECTF--
%s with null:


%s with false:


%s with true:
1

%s with 2:
2

s with 3.5:
3.5

%s with "foo":
foo

%s with []:

Warning: Array to string conversion in %s on line %d
Array

%s with [1]:

Warning: Array to string conversion in %s on line %d
Array

%s with resource:
Resource id #%d

%s with {}:
Object of class stdClass could not be converted to string

d with null:
0

d with false:
0

d with true:
1

d with 2:
2

d with 3.5:
3

d with "foo":
0

d with []:
0

d with [1]:
1

d with resource:
%d

d with {}:

Warning: Object of class stdClass could not be converted to int in %s on line %d
1

u with null:
0

u with false:
0

u with true:
1

u with 2:
2

u with 3.5:
3

u with "foo":
0

u with []:
0

u with [1]:
1

u with resource:
%d

u with {}:

Warning: Object of class stdClass could not be converted to int in %s on line %d
1

f with null:
0.000000

f with false:
0.000000

f with true:
1.000000

f with 2:
2.000000

f with 3.5:
3.500000

f with "foo":
0.000000

f with []:
0.000000

f with [1]:
1.000000

f with resource:
%d.000000

f with {}:

Warning: Object of class stdClass could not be converted to float in %s on line %d
1.000000

c with null:
%0

c with false:
%0

c with true:


c with 2:


c with 3.5:


c with "foo":
%0

c with []:
%0

c with [1]:


c with resource:
%s

c with {}:

Warning: Object of class stdClass could not be converted to int in %s on line %d


x with null:
0

x with false:
0

x with true:
1

x with 2:
2

x with 3.5:
3

x with "foo":
0

x with []:
0

x with [1]:
1

x with resource:
%d

x with {}:

Warning: Object of class stdClass could not be converted to int in %s on line %d
1
