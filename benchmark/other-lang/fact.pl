sub fact{
  my $n = @_[0];
  if($n < 2){
    return 1;
  }
  else{
    return $n * fact($n-1);
  }
}

for($i=0; $i<10000; $i++){
  &fact(100);
}
