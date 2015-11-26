--TEST--
General function test
--FILE--
<?php 

echo "Before function declaration...\n";

function print_something_multiple_times($something,$times)
{
  echo "----\nIn function, printing the string \"$something\" $times times\n";
  for ($i=0; $i<$times; $i++) {
    echo "$i) $something\n";
  }
  echo "Done with function...\n-----\n";
}

function some_other_function()
{
  echo "This is some other function, to ensure more than just one function works fine...\n";
}


echo "After function declaration...\n";

echo "Calling function for the first time...\n";
print_something_multiple_times("This works!",10);
echo "Returned from function call...\n";

echo "Calling the function for the second time...\n";
print_something_multiple_times("This like, really works and stuff...",3);
echo "Returned from function call...\n";

some_other_function();

?>
--EXPECT--

Before function declaration...
After function declaration...
Calling function for the first time...
----
In function, printing the string "This works!" 10 times
0) This works!
1) This works!
2) This works!
3) This works!
4) This works!
5) This works!
6) This works!
7) This works!
8) This works!
9) This works!
Done with function...
-----
Returned from function call...
Calling the function for the second time...
----
In function, printing the string "This like, really works and stuff..." 3 times
0) This like, really works and stuff...
1) This like, really works and stuff...
2) This like, really works and stuff...
Done with function...
-----
Returned from function call...
This is some other function, to ensure more than just one function works fine...
