sub fib{
  my $n = $_[0];
  if($n < 3){
    return 1;
  }
  else{
    return fib($n-1) + fib($n-2);
  }
};

&fib(34);
