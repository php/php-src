sub foo()
{
	my $i=0;
	
	if ($i) {
		my $a = "zeev";
	} else {
		my $b = "andi";
	}
}


sub bar()
{
	foo();
}

for ($i=0; $i<1000000; $i++) {
	bar();
}
