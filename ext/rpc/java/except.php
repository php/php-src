<?
  $stack=new Java("java.util.Stack");
  $stack->push(1);

  #
  # Should succeed and print out "1"
  #
  $result = $stack->pop();
  $ex = java_last_exception_get();
  if (!$ex) print "$result\n";

  #
  # Should fail - note the "@" eliminates the warning
  #
  $result=@$stack->pop();
  $ex=java_last_exception_get();
  if ($ex) print $ex->toString();

  #
  # Reset last exception
  #
  java_last_exception_clear();
?>
