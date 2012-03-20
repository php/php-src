sub fib {
    my($n)=@_;
    if ($n<2) {
	return $n;
    }
    else {
	return fib($n-2)+fib($n-1);
    }
}

print fib(20), "\n";
