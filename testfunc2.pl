sub foo
{
	my $i = shift(@_);
	
	if ($i) {
		my $a = "zeev";
	} else {
		my $b = "andi";
	}
}


sub bar
{
	my $i = shift(@_);
	
	foo($i);
}

for ($i=0; $i<1000000; $i=$i+1) {
	bar($i);
}
