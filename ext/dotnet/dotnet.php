<?
  $stack = new DOTNET("mscorlib","System.Collections.Stack");

  $stack->Push(".Net");
  $stack->Push("Hello ");

  echo $stack->Pop() . $stack->Pop();
?>
